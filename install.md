---
layout: page
title: Install
---

## openSUSE

The RPMs are being published in an [OBS repository][obs] for the latest openSUSE
[Leap][leap] and [Tumbleweed][tw] distributions.

    $ sudo zypper ar https://download.opensuse.org/repositories/home:/mkoutny:/dove-eye/openSUSE_Leap_15.0/home:mkoutny:dove-eye.repo
    $ sudo zypper ar https://download.opensuse.org/repositories/home:/mkoutny:/dove-eye/openSUSE_Tumbleweed/home:mkoutny:dove-eye.repo
    
    $ sudo zypper in dove-eye
    
    $ # after install just launch it
    $ dove-eye

[obs]: https://build.opensuse.org/package/show/home:mkoutny:dove-eye/dove-eye
[leap]: https://download.opensuse.org/repositories/home:/mkoutny:/dove-eye/openSUSE_Leap_15.0/
[tw]: https://download.opensuse.org/repositories/home:/mkoutny:/dove-eye/openSUSE_Tumbleweed/

## Ubuntu (obsolete version 0.1)

There's a PPA repository that packages the application. The installation is as
simple as follows.

    $ sudo add-apt-repository ppa:werkov/ppa
    $ sudo apt-get update
    $ sudo apt-get install dove-eye

    $ # after install just launch it
    $ dove-eye

Currently, [there are builds][pkg] for Ubuntu 14.04 Trusty and Ubuntu 15.04 Vivid.

[pkg]: https://github.com/Werkov/dove-eye-installer/blob/master/README.md

## Windows (version 0.1 with OpenTLD)

32b version build, with OpenTLD tracker, [punk package][win] (sha1: `f86fde54e9d409fd382b4c72d7c0df5dd5dd4410`)

[win]: https://github.com/Werkov/dove-eye/releases/download/v0.1-tld-1/ReleaseTLD.zip

