module View.Log exposing (..)

import Element as Ui
import Model.Log as LogModel
import Time


view : Time.Zone -> LogModel.Log -> Ui.Element msg
view timeZone log =
    Ui.text <| LogModel.toString timeZone log
