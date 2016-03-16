#!/bin/bash

ssh -L 9100:localhost:9102 -L 9101:localhost:9103 -i sla.pem hadoop@$1
