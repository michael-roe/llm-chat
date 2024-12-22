#
# Retrieve the description of a LOINC term from the LOINC server
#

import os
import requests

response = requests.get("https://fhir.loinc.org/CodeSystem/$lookup?system=http://loinc.org&code=8480-6&property=LONG_COMMON_NAME",
      auth=(os.environ.get("LOINC_USER"), os.environ.get("LOINC_PASSWORD"))
    )

if response:
  d = response.json();
  print(d["parameter"][1]["valueString"])
