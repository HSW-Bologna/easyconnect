module View.Common exposing (..)

import Element as Ui exposing (Attribute)
import Element.Border as Border
import Element.Input as Input
import Style


button : List (Attribute msg) -> msg -> Ui.Element msg -> Ui.Element msg
button attrs event content =
    Input.button (attrs ++ Style.inputAttrs ++ [ Ui.width <| Ui.px 64, Ui.height <| Ui.px 64 ])
        { onPress = Just event, label = content }
