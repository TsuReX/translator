#!/bin/sh

echo "RLON $1" `date`>> /home/user/workspace/translator/translator.log
systemctl start man_adc@"$1".service
