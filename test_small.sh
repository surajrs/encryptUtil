#!/bin/bash
hexdump -C plaintextsmall
hexdump -C keyfileplain
cat plaintextsmall |./encryptUtil -n 3 -k keyfileplain >cyphertext
hexdump -C cyphertext
cat cyphertext |./encryptUtil -n 3 -k keyfileplain >plaintextdecoded
hexdump -C plaintextdecoded
