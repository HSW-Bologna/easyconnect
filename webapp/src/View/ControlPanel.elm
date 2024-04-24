module View.ControlPanel exposing (view)

import Element as Ui
import Element.Input as UiInput
import Element.Region exposing (description)
import Image
import Model.Device exposing (Device, getLightGroups)
import Style
import View.Common as Common


view :
    { a | fanState : Maybe Bool, speed : Int, filterState : Maybe Bool, lightState : Maybe ( Bool, Bool, Bool ), devices : List Device }
    -> { b | fanControl : msg, speedControl : Int -> msg, filterControl : msg, lightControl : msg }
    -> Ui.Element msg
view { fanState, speed, filterState, lightState, devices } { fanControl, speedControl, filterControl, lightControl } =
    let
        fanSlider =
            UiInput.slider
                (Ui.width Ui.fill
                    :: Style.inputAttrs
                )
                { onChange = speedControl << round
                , label = UiInput.labelBelow [] <| Ui.text ("Velocita' " ++ String.fromInt (speed + 1))
                , min = 0
                , max = 4
                , step = Just 1
                , value = toFloat speed
                , thumb = UiInput.defaultThumb
                }

        toggleButton toggled control on off =
            Maybe.map
                (\state ->
                    Common.button []
                        control
                        (if state then
                            on

                         else
                            off
                        )
                )
                toggled
                |> Maybe.withDefault Ui.none

        lightButton =
            Maybe.map
                (\ls ->
                    let
                        ( first, second, third ) =
                            ls
                    in
                    case getLightGroups devices of
                        1 ->
                            toggleButton (Just first) lightControl Image.lightOn Image.lightOff

                        2 ->
                            Common.button []
                                lightControl
                                (Ui.el
                                    [ Ui.width Ui.fill
                                    , Ui.height Ui.fill
                                    , Ui.inFront <|
                                        Ui.image [ Ui.moveUp 4.0, Ui.moveLeft 4.0, Ui.height (Ui.px 32) ]
                                            { src =
                                                if first then
                                                    Image.lightOnSrc

                                                else
                                                    Image.lightOffSrc
                                            , description = ""
                                            }
                                    ]
                                 <|
                                    Ui.image [ Ui.centerX, Ui.centerY, Ui.moveDown 8.0, Ui.moveRight 8.0, Ui.height (Ui.px 32) ]
                                        { src =
                                            if second then
                                                Image.lightOnSrc

                                            else
                                                Image.lightOffSrc
                                        , description = ""
                                        }
                                )

                        3 ->
                            Common.button []
                                lightControl
                                (Ui.el
                                    [ Ui.width Ui.fill
                                    , Ui.height Ui.fill
                                    , Ui.inFront <|
                                        Ui.image [ Ui.moveUp 8.0, Ui.moveLeft 8.0, Ui.height (Ui.px 32) ]
                                            { src =
                                                if third then
                                                    Image.lightOnSrc

                                                else
                                                    Image.lightOffSrc
                                            , description = ""
                                            }
                                    , Ui.inFront <|
                                        Ui.image [ Ui.moveDown 25.0, Ui.moveRight 30.0, Ui.height (Ui.px 24) ]
                                            { src =
                                                if first then
                                                    Image.lightOnSrc

                                                else
                                                    Image.lightOffSrc
                                            , description = ""
                                            }
                                    ]
                                 <|
                                    Ui.image [ Ui.centerX, Ui.centerY, Ui.height (Ui.px 28) ]
                                        { src =
                                            if second then
                                                Image.lightOnSrc

                                            else
                                                Image.lightOffSrc
                                        , description = ""
                                        }
                                )

                        _ ->
                            Ui.none
                )
                lightState
                |> Maybe.withDefault Ui.none

        fanButton =
            toggleButton fanState fanControl Image.fanImmission Image.fanOff

        filterButton =
            toggleButton filterState filterControl Image.filtersOn Image.filtersOff
    in
    Ui.column [ Ui.width Ui.fill, Ui.spacing 32, Ui.paddingXY 32 8 ]
        [ Ui.row [ Ui.spaceEvenly ]
            [ lightButton, fanButton, filterButton ]
        , fanSlider
        ]
