Release automation (semantic-release)

What this adds
- .releaserc.json: semantic-release configuration to analyze Conventional Commits, generate release notes, update CHANGELOG.md, tag releases, and create GitHub Releases.
- .github/workflows/release.yml: GitHub Actions workflow triggered by pushes to main, hotfix/*, release/*, and manual dispatch. The release job runs only on main, hotfix/*, and release/*.
- The release job builds the current ESP-IDF v6.1 `esp32` firmware before running semantic-release and attaches its individual `.bin` images and flashing metadata to each GitHub Release. A build or packaging failure stops the job before a release can be published. GitHub's automatically generated source archives remain available.

Firmware assets
- Release assets include the current `open_extraction` ESP32 application build, its bootloader and partition table (and any other binary images referenced by the generated flash arguments), plus `flash_args`, a manifest with the source commit and image checksums, and flashing instructions. Download all assets into the same directory before flashing; the generated arguments use the downloaded filenames.
- These assets represent the firmware currently implemented and buildable in this repository. They are not a claim that production hardware, all planned features, or the intended future controller/satellite device roles are complete.
- Connect an ESP32 over USB/serial, then follow [firmware/FLASHING.md](firmware/FLASHING.md). It describes installing `esptool` and flashing the included images using their generated addresses.

Enabling the automation
1. By default the workflow uses the repository's GITHUB_TOKEN. For commit and tag pushes created by semantic-release, the workflow needs write access to repository contents.
2. For greater control use a personal access token (PAT) with repo write permissions and add it to repository secrets as RELEASE_GITHUB_TOKEN. The PAT should have at least: repo (full control of private repos) or repo:status, repo_deployment, repo:public_repo, repo:invite and workflow write access depending on org settings.
3. Add the secret: Repository > Settings > Secrets > Actions > New repository secret. Name it RELEASE_GITHUB_TOKEN.
4. Merge the PR after the secret is added. The workflow will run on subsequent pushes/merges.

Safety notes
- Do NOT enable or add the secret until a repo admin is ready — enabling allows automated commits/tags/releases.
- The PR is intentionally non-invasive: it uses npx semantic-release so no package.json is required in the repo.
- Releases remain limited to main, hotfix/*, and release/*; manual dispatch remains guarded by the same branch check.

Next steps
- Merge the PR once the token is configured.
- Optionally adjust .releaserc.json (branches, commit message template, release assets).
