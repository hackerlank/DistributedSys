#!/bin/bash

ssh -L 9100:localhost:9104 -L 9101:localhost:9105 -i sla.pem hadoop@$1
