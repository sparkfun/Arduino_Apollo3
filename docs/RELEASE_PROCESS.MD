
# Release Process
The purpose of this document is to document the release process for future reference.

## Preparation
1. Make sure that any desired to mbed are present on the [ambiq-apollo3-arduino](https://github.com/sparkfun/mbed-os-ambiq-apollo3/tree/ambiq-apollo3-arduino) branch in the sparkfun branch of Mbed.
2. Bring all commits and pull requests into the dev branch. If any submodules have been updated, don't forget to update the commit used in the dev branch
3. Github actions will automatically generate the mbed variants after a commit into the dev branch. Allow these actions to finish, and the release-candidate branch will have have the latest commits, and the variants branch will be populated with the latest from the Mbed fork
4. When ready for release, complete a pull request from release-candidate into main

## Drafting the release
5. Create a release in github based on main. Currently this is available, by going to [Arduino_Apollo3 Repo](https://github.com/sparkfun/Arduino_Apollo3) -> Releases -> Draft a new release.
6. Create a tag on the main branch with the version number. Ex, "v2.5.0"
7. Include a description about the release
8. (Optional) Mark as a pre-release, and open a discussion board to collect feedback from the community.

## Upload zip file, and update JSON for Arduino
9. Download the repo at the tag created above `git clone --recursive -b <tagname> https://github.com/sparkfun/Arduino_Apollo3`. Tip: If you forget to include --recursive in the clone, all submodules can be acquired via `git submodule update --init --recursive`.
10. Make sure this directory is named "Arduino_Apollo3"
11. Create a tarball of the release. `tar --exclude-vcs -zcvf Arduino_Apollo3.tar.gz Arduino_Apollo3`. Make sure to include `--exclude-vcs` to exclude gigabytes of git information from the release package.
  WARNING: Do not do this on a windows machine, or unix users will need to run a chmod -x on the file later. 
12. Edit the release created above, and upload the tarball
13. Run a SHA256 chksum on the tarball, and note size. `shasum -a 256 Arduino_Apollo3.tar.gz`
14. Update the [JSON File](https://github.com/sparkfun/Arduino_Apollo3/blob/main/package_sparkfun_apollo3_index.json) with new information. This can be quickly done from the github website ("." to open vscode).
  * Create a new entry, or copy a previous one.
  * Update the version number
  * Update the URL of the tarball
  * Update the checksum with the SHA256 of the tarball
  * Update the size of the poackage

## Verification and Clean-up
15. After updating the json file, restart your arduino IDE (that has the JSON added in the additional board manager preference). The new release should now appear. If not, check for any error messages from arduino about parsing the JSON, and check to make sure no errors were made or steps skipped in the section above.
16. Download and verify the release.
17. (Optional) To reduce possible merge conflicts later, perform a pull request from "main" branch into the "dev" branch so that both have all changes from these steps.


