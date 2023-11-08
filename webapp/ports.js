function wspath(local) {
    var loc = window.location,
        new_uri;
    if (loc.protocol === "https:") {
        new_uri = "wss:";
    } else {
        new_uri = "ws:";
    }

    if (loc.host.length == 0) {
        new_uri += "127.0.0.1:8080";
    } else {
        new_uri += "//" + loc.host;
        //new_uri += "//192.168.43.246";
        new_uri += loc.pathname + local;
    }
    return new_uri;
}

var didCommunicate = true;
var socket;

function connect() {
    socket = new WebSocket(wspath("ws"));

    socket.onopen = function (e) {
        app.ports.connectionError.send(false);
    };

    socket.onmessage = function (e) {
        didCommunicate = true;
        console.log(e)
        try {
            var msg = JSON.parse(e.data);
            for (const property in msg) {
                if (app.ports[property] != undefined) {
                    app.ports[property].send(msg[property]);
                }
            }
        } catch (error) {
            console.log(error);
            console.log(e.data);
        }
    };

    socket.onerror = function (err) {
        console.error("Socket encountered error: ", err.message, "Closing socket");
        socket.close();
    };

    socket.onclose = function (e) {
        console.log("Closed socket " + e);
        app.ports.connectionError.send(true);
        setTimeout(function () {
            console.log("Retry connection");
            connect();
        }, 1000);
    };
}

connect();

setInterval(function() {
  if (didCommunicate) {
      console.log("All good");
    didCommunicate = false;
  } else {
      console.log("No communication!");
    app.ports.connectionError.send(true);
  }
}, 10000);


var app = Elm.Main.init({
    node: document.getElementById("elm-app"),
    flags: 0,
});


app.ports.command.subscribe(function (message) {
    console.log("sending " + JSON.stringify(message))
    if (socket.readyState == WebSocket.OPEN) {
        socket.send(JSON.stringify(message));
    } else {
        socket.addEventListener("open", (event) => {
            socket.send(JSON.stringify(message));
        })
    }
});

