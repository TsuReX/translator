#!/bin/sh

echo "RLOFF $1" `date` >> /home/user/workspace/translator/translator.log
systemctl stop man_adc@"$1".service
