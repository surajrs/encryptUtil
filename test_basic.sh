#!/bin/bash
hexdump -C plaintext
hexdump -C keyfile
cat plaintext |./encryptUtil -n 3 -k keyfile >cyphertext
hexdump -C cyphertext
