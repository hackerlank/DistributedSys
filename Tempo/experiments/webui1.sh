#!/bin/bash

ssh -L 9100:localhost:9100 -L 9101:localhost:9101 -i sla.pem hadoop@$1
