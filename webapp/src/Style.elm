module Style exposing
    ( activeColor
    , borderColor
    , borderRadius
    , borderWidth
    , br
    , buttonAttributes
    , centerWidth
    , defaultAttributes
    , defaultButtonAttributes
    , dimensioneFontNormale
    , dimensioneFontSottotitolo
    , dimensioneFontTitolo
    , hamburgerSize
    , iconFont
    , iconSize
    , logoWidth
    , padding
    , parameterPadding
    , spacing
    , width
    )

import Element as Ui
import Element.Background as Background
import Element.Border as Border
import Element.Font exposing (center)


padding =
    Ui.padding 8


width =
    Ui.width (Ui.maximum 800 Ui.fill)


parameterPadding =
    Ui.padding 16


logoWidth : Ui.Length
logoWidth =
    Ui.shrink


iconFont : Int
iconFont =
    32


dimensioneFontTitolo : Int
dimensioneFontTitolo =
    32


dimensioneFontSottotitolo : Int
dimensioneFontSottotitolo =
    24


dimensioneFontNormale : Int
dimensioneFontNormale =
    16


iconSize : Ui.Length
iconSize =
    Ui.shrink


hamburgerSize : Int
hamburgerSize =
    60


borderWidth =
    Border.width 4


borderColor =
    Border.color (Ui.rgb255 0x00 0x4A 0xAD)


borderRadius =
    Border.rounded 16


spacing =
    Ui.spacing 12


defaultAttributes =
    [ Ui.centerX
    , padding
    , width
    , borderWidth
    , borderColor
    , borderRadius
    , spacing
    ]


activeColor =
    Ui.rgb 0 0.8 0


defaultButtonAttributes =
    [ padding, borderRadius, borderWidth, borderColor, Ui.mouseDown [ Background.color <| Ui.rgb 0 1 0 ] ]


centerWidth : Int
centerWidth =
    300


br : Ui.Element msg
br =
    let
        w =
            centerWidth - 40
    in
    Ui.el [ Ui.centerX, borderColor, Border.width 1, Ui.height (Ui.px 0), Ui.width (Ui.maximum w Ui.fill) ] Ui.none


buttonAttributes : List (Ui.Attribute msg)
buttonAttributes =
    [ Ui.alignLeft
    , Ui.padding 12
    , borderRadius
    , borderWidth
    , borderColor
    , Ui.mouseDown [ Background.color activeColor ]
    ]
