# Repository settings (recommended)

This document describes recommended GitHub repository settings and exact steps maintainers can use to enable them. It is documentation-only: do not apply changes via automation in this PR. Repository defaults: default branch = `develop`.

If your CI provider uses different status check names, replace the placeholders below with your actual check names.

---

## Goals / Rationale

- Protect the default branch(s) from accidental or unchecked changes.
- Ensure code review and CI pass before merging.
- Assign ownership (CODEOWNERS) for sensitive areas.
- Enable security scanning and secret scanning to detect risks early.
- Provide sensible merge strategy to keep history readable.

---

## Branch protection (default branch: develop)

Recommended branch-protection settings (UI steps and gh api examples):

- Branch name pattern: `develop`
- Require pull request reviews before merging: 1 approving review
- Require review from CODEOWNERS: enabled
- Dismiss stale pull request approvals when new commits are pushed: enabled
- Require status checks to pass before merging: enable and list required checks (placeholders below)
- Require linear history: optional — recommend `Require linear history` or prefer squash/rebase merge policy
- Restrict who can push: restrict pushes to maintainers or a team (admins can be exempt)
- Require signed commits: optional — enable if your team uses GPG/Signed commits

Required status checks (replace with your CI job names):
- `ci/build`
- `ci/test`
- `ci/lint`

UI steps (maintainers):
1. Go to Settings → Branches → Branch protection rules → Add rule
2. Enter branch name pattern: `develop`
3. Check: Require pull request reviews before merging → set `Require approving reviews` to 1
4. Check: Require review from Code Owners
5. Check: Dismiss stale pull request approvals when new commits are pushed
6. Check: Require status checks to pass before merging → select the CI checks listed above (or the real names used by your CI)
7. Optionally check: Require linear history
8. Optionally check: Restrict who can push → add teams or users who may push
9. Save changes

gh api example (create branch protection using the REST API):

Note: replace OWNER/REPO, branch name, and status check contexts with correct values.

```bash
# Example: require PR reviews + status checks + code owners
gh api repos/OWNER/REPO/branches/develop/protection --method PUT -F required_status_checks='{"strict":true,"contexts":["ci/build","ci/test","ci/lint"]}' -F enforce_admins=false -F required_pull_request_reviews='{"dismiss_stale_reviews":true,"required_approving_review_count":1,"require_code_owner_reviews":true}' -F restrictions='{"users":[],"teams":[]}'
```

Alternatively use curl:

```bash
curl -X PUT -H "Accept: application/vnd.github+json" \
  -H "Authorization: Bearer $GITHUB_TOKEN" \
  https://api.github.com/repos/OWNER/REPO/branches/develop/protection \
  -d '{
    "required_status_checks": {"strict": true, "contexts": ["ci/build","ci/test","ci/lint"]},
    "enforce_admins": false,
    "required_pull_request_reviews": {"dismiss_stale_reviews": true, "required_approving_review_count": 1, "require_code_owner_reviews": true},
    "restrictions": null
  }'
```

---

## Merge strategy

Recommendation:
- Prefer squash merging for a single logical commit per PR (recommended). Alternatively allow Rebase & merge if you prefer a strict linear history.
- Disable "Create a merge commit" unless your team wants non-linear merges.

UI steps:
1. Settings → General → Merge button → uncheck/check the allowed merge strategies

gh example (enable/disable merge strategies via API):
```bash
# Example: disable merge commits, enable squash
gh api repos/OWNER/REPO -X PATCH -F allow_merge_commit=false -F allow_squash_merge=true -F allow_rebase_merge=true
```

---

## Repository-level security & analysis

Recommended settings to enable (via UI):
- Dependabot alerts & security updates (Settings → Security → Dependabot)
- Secret scanning and push protection (Settings → Security)
- Enable Code scanning (GitHub Advanced Security) if available
- Enable Dependabot for dependency updates

Note: enabling some features requires repository admin privileges or GitHub Advanced Security license.

---

## Required repository secrets (examples)

Do not store plaintext secrets in the repo. Use GitHub Actions secrets when needed. Common secrets:
- `RELEASE_GITHUB_TOKEN` (if you use a release workflow that requires a dedicated token)
- `NPM_TOKEN`, `PYPI_API_TOKEN`, or other package registry tokens used by CI

Example gh command to set a secret (requires repo admin and gh authenticated):

```bash
# Set a secret using the gh CLI
printf "my-secret-value" | gh secret set SECRET_NAME --repo OWNER/REPO --body -
```

Or using the API (requires encryption step): see GitHub docs for `actions/secrets`.

---

## CODEOWNERS and ownership guidance

- CODEOWNERS (existing at `.github/CODEOWNERS`) assigns reviewers automatically for paths. Keep it up to date.
- With "Require review from CODEOWNERS" enabled, PRs touching owned paths must be approved by owners before merge.
- Keep sensitive areas (CI workflows, release scripts, security-critical code) owned by a team, not a single user.

Example (current repository): `.github/CODEOWNERS` sets `@jmartens` as default owner.

---

## Admin overrides and cautions

- `enforce_admins`: setting this to `true` prevents admins from bypassing protections. Use with caution: it can lock out emergency fixes if misconfigured.
- Start with `enforce_admins: false` while rolling out rules and verify team workflows.
- Strict push restrictions and signed-commits requirements may block automation; review CI bots and service accounts before enabling.

---

## Checklist for maintainers to apply settings

- [ ] Review and replace required status check names with actual CI job names.
- [ ] Apply branch protection for `develop` (UI or gh api).
- [ ] Configure merge strategies to team preference (squash recommended).
- [ ] Enable security & Dependabot features.
- [ ] Ensure CODEOWNERS is current and owners are reachable.
- [ ] Add required secrets via `gh secret set` or the UI.

---

## Next steps (this PR)

This PR adds documentation only. A repository admin must apply the settings via the UI or run the gh/api commands above. If desired, an automation PR can be created later to apply the settings using a GitHub App with appropriate permissions.

---

## References
- GitHub branch protection docs: [Configuring protected branches](https://docs.github.com/en/repositories/configuring-branches-and-merges-in-your-repository/configuring-protected-branches)
- GitHub REST API: Branch protection: [Branch protection REST API](https://docs.github.com/en/rest/branches/branch-protection)
- GitHub Actions Secrets: [Encrypted secrets](https://docs.github.com/en/actions/security-guides/encrypted-secrets)

---

File generated by: docs/REPOSITORY_SETTINGS.md (documentation-only change for issue #57)
