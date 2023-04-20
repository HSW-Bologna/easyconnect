module View.ControlPanel exposing (view)

import Element as Ui
import Element.Input as UiInput
import Image
import Style
import View.Common as Common


view :
    { a | fanState : Maybe Bool, speed : Int, filterState : Maybe Bool }
    -> { b | fanControl : msg, speedControl : Int -> msg, filterControl : msg }
    -> Ui.Element msg
view { fanState, speed, filterState } { fanControl, speedControl, filterControl } =
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

        fanButton =
            toggleButton fanState fanControl Image.fanImmission Image.fanOff

        filterButton =
            toggleButton filterState filterControl Image.filtersOn Image.filtersOff
    in
    Ui.column [ Ui.width Ui.fill, Ui.spacing 32, Ui.paddingXY 32 8 ]
        [ Ui.row [ Ui.spaceEvenly ]
            [ fanButton, filterButton ]
        , fanSlider
        ]
