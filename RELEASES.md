Release automation (semantic-release)

What this adds
- .releaserc.json: semantic-release configuration to analyze Conventional Commits, generate release notes, update CHANGELOG.md, tag releases, and create GitHub Releases.
- .github/workflows/release.yml: GitHub Actions workflow that runs semantic-release on pushes to the default branch (develop) and on manual dispatch.

Enabling the automation
1. By default the workflow uses the repository's GITHUB_TOKEN. For commit and tag pushes created by semantic-release, the workflow needs write access to repository contents.
2. For greater control use a personal access token (PAT) with repo write permissions and add it to repository secrets as RELEASE_GITHUB_TOKEN. The PAT should have at least: repo (full control of private repos) or repo:status, repo_deployment, repo:public_repo, repo:invite and workflow write access depending on org settings.
3. Add the secret: Repository > Settings > Secrets > Actions > New repository secret. Name it RELEASE_GITHUB_TOKEN.
4. Merge the PR after the secret is added. The workflow will run on subsequent pushes/merges.

Safety notes
- Do NOT enable or add the secret until a repo admin is ready — enabling allows automated commits/tags/releases.
- The PR is intentionally non-invasive: it uses npx semantic-release so no package.json is required in the repo.

Next steps
- Merge the PR once the token is configured.
- Optionally adjust .releaserc.json (branches, commit message template, assets to commit).
