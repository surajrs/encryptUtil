#!/bin/bash

#copy executable to testing
cp encryptUtil test/

#encode the executable
cat test/encryptUtil |./encryptUtil -n 5 -k keyfileplain >test/encryptUtil_encoded

echo "comparing the original and encoded"
diff encryptUtil test/encryptUtil_encoded
#decode back
cat test/encryptUtil_encoded |./encryptUtil -n 15 -k keyfileplain >test/encryptUtil_decoded

# diff with the original
echo "comparing the final decoded file with original file"
diff encryptUtil test/encryptUtil_decoded

