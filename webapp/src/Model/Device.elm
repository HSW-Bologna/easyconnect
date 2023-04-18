module Model.Device exposing (..)

import Bitwise
import Json.Decode as Decode


type DeviceClass
    = Unknown
    | Light { group : Int, on : Bool }
    | Uvc { group : Int, on : Bool, feedback : Bool }
    | Esf { group : Int, on : Bool, feedback : Bool }
    | Gas { group : Int, on : Bool }
    | Fan { group : Int, speed : Int, on : Bool }
    | PressureTemperature { group : Int, pressure : Int, temperature : Int }
    | TemperatureHumidity { group : Int, temperature : Int, humidity : Int }
    | PressureTemperatureHumidity { group : Int, pressure : Int, temperature : Int, humidity : Int }


type alias Device =
    { class : DeviceClass
    , address : Int
    , serial : Int
    , alarm : Bool
    }


getGroup : DeviceClass -> Maybe Int
getGroup class =
    case class of
        Unknown ->
            Nothing

        Light { group } ->
            Just group

        Uvc { group } ->
            Just group

        Esf { group } ->
            Just group

        Gas { group } ->
            Just group

        Fan { group } ->
            Just group

        PressureTemperature { group } ->
            Just group

        TemperatureHumidity { group } ->
            Just group

        PressureTemperatureHumidity { group } ->
            Just group


deviceStateDecoder : Decode.Decoder Device
deviceStateDecoder =
    let
        toBool int =
            int > 0

        classHelper : Int -> Decode.Decoder DeviceClass
        classHelper classCode =
            let
                intMode =
                    Bitwise.shiftRightBy 4 classCode

                intGroup =
                    Bitwise.and classCode 0x0F + 1
            in
            case intMode of
                1 ->
                    Decode.map (\on -> Light { group = intGroup, on = on }) (Decode.map toBool <| Decode.field "out" Decode.int)

                2 ->
                    Decode.map2 (\on fb -> Uvc { group = intGroup, on = on, feedback = fb })
                        (Decode.map toBool <| Decode.field "out" Decode.int)
                        (Decode.map toBool <| Decode.field "fb" Decode.int)

                3 ->
                    Decode.map2 (\on fb -> Esf { group = intGroup, on = on, feedback = fb })
                        (Decode.map toBool <| Decode.field "out" Decode.int)
                        (Decode.map toBool <| Decode.field "fb" Decode.int)

                4 ->
                    Decode.map (\on -> Gas { group = intGroup, on = on }) (Decode.map toBool <| Decode.field "out" Decode.int)

                0x0101 ->
                    Decode.map2 (\press temp -> PressureTemperature { group = intGroup, pressure = press, temperature = temp })
                        (Decode.field "press" Decode.int)
                        (Decode.field "temp" Decode.int)

                0x0102 ->
                    Decode.map2 (\temp hum -> TemperatureHumidity { group = intGroup, temperature = temp, humidity = hum })
                        (Decode.field "temp" Decode.int)
                        (Decode.field "hum" Decode.int)

                0x0103 ->
                    Decode.map3 (\press temp hum -> PressureTemperatureHumidity { group = intGroup, pressure = press, temperature = temp, humidity = hum })
                        (Decode.field "press" Decode.int)
                        (Decode.field "temp" Decode.int)
                        (Decode.field "hum" Decode.int)

                0x0201 ->
                    Decode.map2 (\speed on -> Fan { group = intGroup, speed = speed, on = on })
                        (Decode.field "speed" Decode.int)
                        (Decode.map toBool <| Decode.field "out" Decode.int)

                _ ->
                    Decode.succeed Unknown
    in
    Decode.map4 Device
        (Decode.field "class" Decode.int
            |> Decode.andThen classHelper
        )
        (Decode.field "ip" Decode.int)
        (Decode.field "sn" Decode.int)
        (Decode.map toBool <| Decode.field "alarm" Decode.int)
