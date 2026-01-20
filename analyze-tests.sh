#!/bin/bash
cd /home/smueller/Dropbox/Research/pebl/pebl/upload-battery

echo "==========================="
echo "2-ALTERNATIVE FORCED CHOICE"
echo "==========================="
for test in ANT BST crt dotjudgment evenodd flanker globallocal luckvogel manikin matrixrotation oddball simon wpt TNT; do
  echo "$test"
done

echo ""
echo "==========================="
echo "GO/NO-GO (SINGLE KEY)"
echo "==========================="
for test in clocktest pcpt pcpt-ax; do
  echo "$test"
done

echo ""
echo "==========================="
echo "NEEDS INVESTIGATION"
echo "==========================="
for test in corsi gonogo maze ppvt ptrails stroop-color stroop-number stroop-vic; do
  pbl=$(find "$test" -name "*.pbl" -type f ! -name "*lib*" | head -1)
  if [ -n "$pbl" ]; then
    echo -n "$test: "
    if grep -q "WaitForLayoutResponse" "$pbl"; then
      echo "Uses WaitForLayoutResponse"
    else
      echo "No WaitForLayoutResponse found (may use custom response)"
    fi
  fi
done
