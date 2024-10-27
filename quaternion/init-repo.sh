#!/usr/bin/env bash

set -euo pipefail

# Check authentication.
gh auth status

# Check existing PR.
if gh pr list | grep --quiet Feedback
then
    echo "Repository is already initialized (PR exists)."
    exit 0
fi

# Create "feedback" branch.
if git branch | grep --quiet feedback
then
    echo "Branch \"feedback\" is already exists."
else
    git checkout -b feedback
    git push -u origin feedback
    git checkout main
fi

# Fixup workflow.
sed -n '/- push/!p' .github/workflows/classroom.yml > temp.yml
mv temp.yml .github/workflows/classroom.yml

# Patch repository.
git commit -am "[SCRIPT] Fixup workflow."
git push

# Create PR.
REPO_NAME=$(gh repo view --json name -q ".name")
gh repo set-default skkv-cpp/$REPO_NAME
gh pr create --base feedback --title "Feedback" --body "Feedback PR for reviewers." --draft
