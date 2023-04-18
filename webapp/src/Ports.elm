port module Ports exposing (..)

import Json.Decode as Decode
import Json.Encode as Encode

-- PORTS


port dataUpdate : (Decode.Value -> msg) -> Sub msg

