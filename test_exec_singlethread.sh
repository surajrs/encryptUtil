#!/bin/bash

#copy executable to testing
cp encryptUtil test/

#encode the executable
cat test/encryptUtil |./encryptUtil -n 1 -k keyfile >test/encryptUtil_encoded

echo "comparing the final decoded file with original file"
diff encryptUtil test/encryptUtil_encoded

#decode back
cat test/encryptUtil_encoded |./encryptUtil -n 1 -k keyfile >test/encryptUtil_decoded

# diff with the original
echo "comparing the final decoded file with original file"
diff encryptUtil test/encryptUtil_decoded

