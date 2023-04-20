module View.Common exposing (..)

import Element as Ui exposing (Attribute)
import Element.Border as Border
import Element.Input as Input
import Style


button : List (Attribute msg) -> msg -> Ui.Element msg -> Ui.Element msg
button attrs event content =
    Input.button (attrs ++ Style.inputAttrs)
        { onPress = Just event, label = content }
