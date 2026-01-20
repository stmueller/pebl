#!/bin/bash
cd /home/smueller/Dropbox/Research/pebl/pebl/upload-battery
for test in ANT BST clocktest corsi crt dotjudgment evenodd flanker globallocal gonogo luckvogel manikin matrixrotation maze oddball pcpt pcpt-ax ppvt ptrails simon stroop-color stroop-number stroop-vic TNT wpt; do
  echo "=== $test ==="
  schema=$(find "$test/params" -name "*.schema.json" 2>/dev/null | head -1)
  if [ -n "$schema" ]; then
    grep -A5 "responsemode" "$schema" | head -10
  else
    echo "No schema found"
  fi
  echo ""
done
