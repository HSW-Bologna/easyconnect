port module Ports exposing (..)

import Json.Decode as Decode
import Json.Encode as Encode



-- PORTS


port update : (Decode.Value -> msg) -> Sub msg


port devices : (Decode.Value -> msg) -> Sub msg


port logs : (Decode.Value -> msg) -> Sub msg


port connectionError : (Bool -> msg) -> Sub msg


port command : Encode.Value -> Cmd msg


requestLogs : Int -> Cmd msg
requestLogs num =
    command <| Encode.object [ ( "logs", Encode.int num ) ]


setLight : ( Bool, Bool, Bool ) -> Cmd msg
setLight state =
    command <|
        Encode.object
            [ ( "light"
              , Encode.int
                    (case state of
                        ( True, True, True ) ->
                            0x07

                        ( True, True, False ) ->
                            0x03

                        ( True, False, False ) ->
                            0x01

                        ( False, True, False ) ->
                            0x02

                        _ ->
                            0
                    )
              )
            ]


setFan : Bool -> Cmd msg
setFan state =
    command <| Encode.object [ ( "fan", Encode.bool state ) ]


setFilter : Bool -> Cmd msg
setFilter state =
    command <| Encode.object [ ( "filt", Encode.bool state ) ]


setSpeed : Int -> Cmd msg
setSpeed speed =
    command <| Encode.object [ ( "speed", Encode.int speed ) ]
