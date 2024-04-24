module Model.Log exposing (..)

import Json.Decode as Decode
import Time


type alias Log =
    { timestamp : Int
    , code : Int
    , description : Int
    , target : Int
    }


logDecoder : Decode.Decoder Log
logDecoder =
    Decode.map4 Log
        (Decode.field "ts" Decode.int)
        (Decode.field "code" Decode.int)
        (Decode.field "desc" Decode.int)
        (Decode.field "addr" Decode.int)


logsDecoder : Decode.Decoder (List Log)
logsDecoder =
    Decode.list logDecoder


toString : Time.Zone -> Log -> String
toString zone { timestamp, code, description, target } =
    let
        time =
            Time.millisToPosix (timestamp * 1000)

        tellMeWhy m =
            case m of
                Time.Jan ->
                    1

                Time.Feb ->
                    2

                Time.Mar ->
                    3

                Time.Apr ->
                    4

                Time.May ->
                    5

                Time.Jun ->
                    6

                Time.Jul ->
                    7

                Time.Aug ->
                    8

                Time.Sep ->
                    9

                Time.Oct ->
                    10

                Time.Nov ->
                    11

                Time.Dec ->
                    12

        eventDesc =
            case code of
                0 ->
                    "Accensione"

                1 ->
                    "Errore di comunicazione: dispositivo " ++ String.fromInt target

                2 ->
                    "Allarme " ++ String.fromInt description ++ ": dispositivo" ++ String.fromInt target

                _ ->
                    ""
    in
    "["
        ++ String.fromInt (Time.toHour zone time)
        ++ ":"
        ++ String.fromInt (Time.toMinute zone time)
        ++ ":"
        ++ String.fromInt (Time.toSecond zone time)
        ++ " "
        ++ String.fromInt (Time.toDay zone time)
        ++ "/"
        ++ String.fromInt (Time.toMonth zone time |> tellMeWhy)
        ++ "/"
        ++ String.fromInt (Time.toYear zone time)
        ++ "] "
        ++ eventDesc
