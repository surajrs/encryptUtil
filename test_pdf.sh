#!/bin/bash

#copy executable to testing

#encode the executable
cat test/Core_v5.0.pdf |./encryptUtil -n 15 -k keyfile >test/Core_v5.0.pdf.enc

echo "comparing the final decoded file with original file"
diff test/Core_v5.0.pdf.enc test/Core_v5.0.pdf

#decode back
cat test/Core_v5.0.pdf.enc |./encryptUtil -n 15 -k keyfile >test/Core_v5.0.dec.pdf

# diff with the original
echo "comparing the final decoded file with original file"
diff test/Core_v5.0.pdf test/Core_v5.0.dec.pdf

rm test/Core_v5.0.pdf.enc

