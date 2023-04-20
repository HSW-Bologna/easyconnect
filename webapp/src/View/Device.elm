module View.Device exposing (view)

import Element as Ui
import Element.Border as UiBorder
import Image
import Model.Device as DeviceModel


view : DeviceModel.Device -> Ui.Element msg
view { class, address, serial, alarm } =
    let
        leftColumn =
            Ui.column [ Ui.height Ui.fill, Ui.width <| Ui.fillPortion 2, Ui.padding 4 ]

        simpleOutput on img fb =
            [ leftColumn
                [ outputView on
                , alarmsView alarm fb
                ]
            , deviceIcon img
            , metadataView class address serial
            ]
    in
    Ui.row [ Ui.height <| Ui.minimum 48 Ui.shrink, Ui.width Ui.fill, Ui.spacing 16 ] <|
        case class of
            DeviceModel.Unknown ->
                [ leftColumn [ alarmsView alarm Nothing ]
                , deviceIcon <| Ui.text "???"
                , metadataView class address serial
                ]

            DeviceModel.Light { on } ->
                simpleOutput on Image.light Nothing

            DeviceModel.Uvc { on, feedback } ->
                simpleOutput on Image.uvc (Just feedback)

            DeviceModel.Esf { on, feedback } ->
                simpleOutput on Image.esf (Just feedback)

            DeviceModel.Gas { on } ->
                simpleOutput on Image.gas Nothing

            DeviceModel.Fan { on, speed, group } ->
                [ leftColumn
                    [ Ui.el [ Ui.alignTop, Ui.alignRight ] <|
                        Ui.text ("Fan: " ++ String.fromInt speed ++ "%, " ++ outputToString on)
                    , alarmsView alarm Nothing
                    ]
                , deviceIcon
                    (if group == 1 then
                        Image.fanImmission

                     else
                        Image.fanSiphoning
                    )
                , metadataView class address serial
                ]

            DeviceModel.PressureTemperature { pressure, temperature } ->
                [ leftColumn
                    [ Ui.el [ Ui.alignTop, Ui.alignRight ] <|
                        Ui.text (String.fromInt temperature ++ " C, " ++ String.fromInt pressure ++ "Pa")
                    , alarmsView alarm Nothing
                    ]
                , deviceIcon Image.pressureTemperature
                , metadataView class address serial
                ]

            DeviceModel.TemperatureHumidity { temperature, humidity } ->
                [ leftColumn
                    [ Ui.el [ Ui.alignTop, Ui.alignRight ] <|
                        Ui.text (String.fromInt temperature ++ " C, " ++ String.fromInt humidity ++ "%")
                    , alarmsView alarm Nothing
                    ]
                , deviceIcon Image.temperatureHumidity
                , metadataView class address serial
                ]

            DeviceModel.PressureTemperatureHumidity { pressure, temperature, humidity } ->
                [ leftColumn
                    [ Ui.el [ Ui.alignTop, Ui.alignRight ] <|
                        Ui.text (String.fromInt temperature ++ " C, " ++ String.fromInt humidity ++ "%, " ++ String.fromInt pressure ++ "Pa")
                    , alarmsView alarm Nothing
                    ]
                , deviceIcon Image.pressureTemperatureHumidity
                , metadataView class address serial
                ]


alarmsView : Bool -> Maybe Bool -> Ui.Element msg
alarmsView alarms feedback =
    Ui.el [ Ui.alignBottom, Ui.alignRight ] <|
        Ui.text <|
            (Maybe.map
                (\fb ->
                    "FB: "
                        ++ (if fb then
                                "OK "

                            else
                                "KO "
                           )
                )
                feedback
                |> Maybe.withDefault ""
            )
                ++ "All.: "
                ++ (if alarms then
                        "OK"

                    else
                        "KO"
                   )


outputView : Bool -> Ui.Element msg
outputView output =
    Ui.el [ Ui.alignTop, Ui.alignRight ] <|
        Ui.text ("Output: " ++ outputToString output)


metadataView : DeviceModel.DeviceClass -> Int -> Int -> Ui.Element msg
metadataView class address serial =
    let
        stringGroup =
            DeviceModel.getGroup class |> Maybe.map String.fromInt |> Maybe.withDefault "-"
    in
    Ui.column [ Ui.height Ui.fill, Ui.width <| Ui.fillPortion 2, Ui.padding 4 ]
        [ Ui.el [ Ui.alignTop ] <| Ui.text ("IP: " ++ String.fromInt address ++ " Gr:" ++ stringGroup)
        , Ui.el [ Ui.alignBottom ] <| Ui.text ("SN: " ++ String.fromInt serial)
        ]


deviceIcon : Ui.Element msg -> Ui.Element msg
deviceIcon =
    Ui.el
        [ UiBorder.width 2
        , UiBorder.color (Ui.rgb255 210 210 210)
        , UiBorder.rounded 8
        , Ui.centerX
        , Ui.centerY
        , Ui.width <| Ui.px 56
        , Ui.height <| Ui.px 56
        ]
        << Ui.el [ Ui.centerX, Ui.centerY ]


outputToString : Bool -> String
outputToString out =
    if out then
        "ON"

    else
        "OFF"
