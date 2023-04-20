module Main exposing (Model, init, main, update, view)

import Browser
import Element as Ui
import Element.Border as Border
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
import Model.Log exposing (Log, logsDecoder)
import Ports
import Style
import Task
import Time
import View.ControlPanel
import View.Device as DeviceView
import View.Log as LogView



-- MAIN


main : Program Int Model Msg
main =
    Browser.element { init = init, update = update, view = view, subscriptions = subscriptions }



-- MODEL


type alias Model =
    { messageToast : MessageToast Msg
    , devices : List DeviceModel.Device
    , timeZone : Time.Zone
    , logStart : Int
    , newLogStart : Int
    , logs : List Log
    , speed : Int
    , fanState : Maybe Bool
    , filterState : Maybe Bool
    }


init : Int -> ( Model, Cmd Msg )
init _ =
    ( { messageToast = MessageToast.initWithConfig MessageToastBoilerplate { toastsToShow = 4, delayInMs = 10000 }
      , devices = []
      , timeZone = Time.utc
      , logStart = 0
      , newLogStart = 0
      , logs = []
      , fanState = Nothing
      , filterState = Nothing
      , speed = 0
      }
    , Cmd.batch [ Ports.requestLogs 0 ]
    )



-- MESSAGES


type Msg
    = DevicesUpdate Decode.Value
    | StateUpdate Decode.Value
    | Logs Decode.Value
    | MessageToastBoilerplate (MessageToast Msg)
    | UpdateLogs Int
    | ToggleFan
    | ToggleFilter
    | ChangeSpeed Int
    | ErroreConnessione Bool



-- UPDATE


update : Msg -> Model -> ( Model, Cmd Msg )
update msg model =
    case msg of
        ErroreConnessione True ->
            ( { model | messageToast = errorToast model "Errore di rete!" }, Cmd.none )

        ErroreConnessione False ->
            ( model, Cmd.none )

        MessageToastBoilerplate updatedMessageToast ->
            -- Only needed to re-assign the updated MessageToast to the model.
            ( { model | messageToast = updatedMessageToast }, Cmd.none )

        DevicesUpdate value ->
            let
                newDevices =
                    Decode.decodeValue
                        (Decode.list DeviceModel.deviceStateDecoder)
                        value
                        |> Result.withDefault model.devices

                newFanState =
                    DeviceModel.getFanState newDevices

                newFilterState =
                    DeviceModel.getFilterState newDevices
            in
            ( { model | devices = newDevices, fanState = newFanState, filterState = newFilterState }, Cmd.none )

        StateUpdate value ->
            let
                newSpeed =
                    Decode.decodeValue (Decode.field "speed" Decode.int) value |> Result.withDefault model.speed
            in
            ( { model | speed = newSpeed }, Cmd.none )

        Logs value ->
            let
                newLogs =
                    Decode.decodeValue logsDecoder value |> Result.withDefault []

                newLogStart =
                    if List.length newLogs > 0 then
                        model.newLogStart

                    else
                        model.logStart
            in
            ( { model
                | logs =
                    if List.length newLogs > 0 then
                        newLogs

                    else
                        model.logs
                , logStart = newLogStart
              }
            , Cmd.none
            )

        UpdateLogs delta ->
            let
                newLogStart =
                    if model.logStart + delta > 0 then
                        model.logStart + delta

                    else
                        0
            in
            ( { model | newLogStart = newLogStart }
            , Ports.requestLogs newLogStart
            )

        ToggleFan ->
            let
                newFanState =
                    Maybe.map (\x -> not x) model.fanState
            in
            ( { model | fanState = newFanState }, Maybe.map Ports.setFan newFanState |> Maybe.withDefault Cmd.none )

        ToggleFilter ->
            let
                newFilterState =
                    Maybe.map (\x -> not x) model.filterState
            in
            ( { model | filterState = newFilterState }, Maybe.map Ports.setFilter newFilterState |> Maybe.withDefault Cmd.none )

        ChangeSpeed speed ->
            ( { model | speed = speed }, Ports.setSpeed speed )


subscriptions : Model -> Sub Msg
subscriptions model =
    Sub.batch <|
        [ Ports.connectionError ErroreConnessione
        , Ports.devices DevicesUpdate
        , Ports.update StateUpdate
        , Ports.logs Logs
        , MessageToast.subscriptions model.messageToast
        ]



-- VIEW


view : Model -> Html Msg
view model =
    let
        logButton attrs event label =
            Input.button (attrs ++ [ Border.width 2, Style.borderColor, Border.rounded 4, Ui.padding 8 ]) { onPress = Just event, label = Ui.text label }
    in
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
                , Ui.height <| Ui.minimum 320 Ui.fill
                , Ui.padding 16
                , Style.borderColor
                , Style.borderRadius
                , Style.borderWidth
                ]
            <|
                Ui.column [ Ui.width Ui.fill, Ui.height Ui.fill, Ui.spacing 32 ] <|
                    [ Ui.el [ Ui.centerX ] Image.logo
                    , View.ControlPanel.view model { fanControl = ToggleFan, speedControl = ChangeSpeed, filterControl = ToggleFilter }
                    , Ui.el [ Ui.scrollbarY, Ui.width Ui.fill, Ui.height <| Ui.px 480 ] <|
                        Ui.column [ Ui.width Ui.fill, Ui.spacing 24 ] <|
                            List.map DeviceView.view model.devices
                    , Ui.column [ Ui.width Ui.fill, Ui.paddingEach { top = 16, bottom = 0, left = 0, right = 0 } ]
                        [ Ui.row [ Ui.width Ui.fill, Ui.centerX, Ui.paddingXY 0 16 ] <|
                            [ logButton [ Ui.alignLeft ] (UpdateLogs -50) "<-"
                            , Ui.el [ Ui.centerX ] <|
                                Ui.text <|
                                    String.fromInt (model.logStart + 1)
                                        ++ "-"
                                        ++ String.fromInt (model.logStart + List.length model.logs)
                            , logButton [ Ui.alignRight ] (UpdateLogs 50) "->"
                            ]
                        , Ui.el [ Ui.scrollbarY, Ui.width Ui.fill, Ui.height <| Ui.px 480 ] <|
                            Ui.column [ Ui.width Ui.fill, Ui.spacing 24 ] <|
                                List.map (LogView.view model.timeZone) model.logs
                        ]
                    ]


errorToast : Model -> String -> MessageToast Msg
errorToast { messageToast } error =
    messageToast
        |> MessageToast.danger
        |> MessageToast.withMessage
            error
