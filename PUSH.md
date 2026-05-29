# GitHub push (when SSH is configured)

Local commit is ready on branch `main`. Push failed because `wispsync_vps_root` is not registered on GitHub for `cjevolutions`.

## 1. Add deploy key or SSH key to GitHub

Public key (add at https://github.com/settings/keys or as a deploy key on the GooseEye repo):

```
ssh-ed25519 AAAAC3NzaC1lZDI1NTE5AAAAIGfDvNifV1ixXtF5efqg6CbcGE1TtKWPQFjx46fg+QYU wispsync vps root
```

## 2. Push

```bash
export GIT_SSH_COMMAND='ssh -i /Users/josey/.ssh/wispsync_vps_root -o IdentitiesOnly=yes'
cd /Users/josey/Documents/Dev/GooseEyeApp/gooseeye-hardware
git push -u origin main
```

If the remote already has commits, reconcile first (`git pull --rebase origin main`) before pushing.

## Re-authenticate GitHub CLI (alternative)

```bash
gh auth login -h github.com
cd /Users/josey/Documents/Dev/GooseEyeApp/gooseeye-hardware
gh repo sync  # or: git push via HTTPS credential helper
```
