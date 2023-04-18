module Main exposing (Model, init, main, update, view)

import Browser
import Element as Ui
import Element.Font as Font
import Element.Input as Input
import Html exposing (Html)
import Html.Attributes as Attributes
import Http
import Image
import Json.Decode as Decode
import Json.Encode as Encode
import MessageToast exposing (MessageToast, success)
import Model.Device as DeviceModel
import Ports
import Style
import Time
import View.Device as DeviceView



-- MAIN


main : Program Int Model Msg
main =
    Browser.element { init = init, update = update, view = view, subscriptions = subscriptions }



-- MODEL


type alias Model =
    { messageToast : MessageToast Msg
    , devices : List DeviceModel.Device
    }


init : Int -> ( Model, Cmd Msg )
init _ =
    ( { messageToast = MessageToast.initWithConfig MessageToastBoilerplate { toastsToShow = 4, delayInMs = 10000 }
      , devices = []
      }
    , Cmd.none
    )



-- MESSAGES


type Msg
    = DataUpdate Decode.Value
    | MessageToastBoilerplate (MessageToast Msg)
    | NoOp



-- UPDATE


update : Msg -> Model -> ( Model, Cmd Msg )
update msg model =
    case msg of
        MessageToastBoilerplate updatedMessageToast ->
            -- Only needed to re-assign the updated MessageToast to the model.
            ( { model | messageToast = updatedMessageToast }, Cmd.none )

        DataUpdate value ->
            let
                newDevices =
                    Decode.decodeValue
                        (Decode.list DeviceModel.deviceStateDecoder)
                        value
            in
            ( { model | devices = Result.withDefault model.devices newDevices }, Cmd.none )

        NoOp ->
            ( model, Cmd.none )


subscriptions : Model -> Sub Msg
subscriptions model =
    Sub.batch <|
        [ Ports.dataUpdate DataUpdate
        , MessageToast.subscriptions model.messageToast
        ]



-- VIEW


view : Model -> Html Msg
view model =
    Ui.layoutWith
        { options =
            [ Ui.focusStyle
                { borderColor = Nothing
                , backgroundColor = Nothing
                , shadow = Nothing
                }
            ]
        }
        [ model.messageToast
            |> MessageToast.overwriteMessageAttributes [ Attributes.style "white-space" "break-spaces", Attributes.style "overflow" "none" ]
            |> MessageToast.overwriteToastAttributes [ Attributes.style "font-size" "1rem" ]
            |> MessageToast.view
            |> Ui.html
            |> Ui.inFront
        , Font.size Style.dimensioneFontNormale
        ]
    <|
        Ui.el [ Ui.width Ui.fill, Ui.height Ui.fill, Ui.padding 16 ] <|
            Ui.el
                [ Ui.centerX
                , Ui.centerY
                , Ui.width <| Ui.maximum 480 Ui.fill
                , Ui.height <| Ui.minimum 320 Ui.shrink
                , Ui.padding 16
                , Style.borderColor
                , Style.borderRadius
                , Style.borderWidth
                ]
            <|
                Ui.column [ Ui.width Ui.fill, Ui.height Ui.fill, Ui.spacing 32 ] <|
                    [ Ui.el [ Ui.centerX ] Image.logo
                    , Ui.el [ Ui.scrollbarY, Ui.width Ui.fill, Ui.height <| Ui.px 480 ] <|
                        Ui.column [ Ui.width Ui.fill, Ui.spacing 24 ] <|
                            List.map DeviceView.view model.devices
                    ]
