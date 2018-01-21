## Description

ActivityFox is a small KDE program that acts as a workaround to allow Firefox, Google Chrome and Chromium-based browsers to
play nice with KDE Plasma Activities.

ActivityFox lets you use separate browser instances for each activity, keeping tabs related to that activity. ActivityFox starts the browser with a profile linked to your current activity, closes the browser when you close the activity and automatically restores it when you open it again.

## Usage

When run without specifying command line arguments, ActivityFox starts Firefox with a profile named as your current activity id, creating it in a default profile directory if it doesn't exist.

To launch Google Chrome or Chromium with a profile linked to your current activity, use the following commands:

```bash
activityfox --google-chrome
activityfox --chromium
```

If your browser is located in a location different from the default one, you should specify the location of a binary file:

```bash
activityfox --google-chrome --bin-path /usr/lib/firefox/firefox
activityfox --google-chrome --bin-path /opt/google/chrome/google-chrome
activityfox --chromium --bin-path /usr/lib/chromium-browser/chromium-browser
```
In case your profiles are located in a different directory:

```bash
activityfox --google-chrome --profile-dir /home/<user>/.mozilla/firefox
activityfox --google-chrome --profile-dir /home/<user>/.config/google-chrome
activityfox --chromium --profile-dir /home/<user>/.config/chromium
```

If you want to launch your browser with a specific profile, use:

```bash
activityfox profile_name
```
To make ActivityFox create profiles for new activities based on a template profile, first create it:

```bash
activityfox template_profile_name
```

Set it up by making your browser show tabs from the last time, then use this command to automatically create new profiles based on your template:

```bash
activityfox --template-profile-name template_profile_name
```

You may create a desktop file with this command and place it on your desktop or in your launcher menu. This way, in every new activity, you will be able to open a browser instance linked to that activity with an automatically created profile based on your template.

## Hide ActivityFox window

Creating a visible window is required to make the session manager call our application on session saving, but it's possible to hide it with KWin rules.

Right-click the title-bar of the window, select More Actions > Special Application Settings.
In the window rules settings that pop up, set:

- Size & Position > Minimized to Apply Initially, Yes
- Arrangement & Access > Skip Taskbar to Apply initially, Yes
- Arrangement & Access > Skip Pager to Apply initially, Yes
- Arrangement & Access > Skip Switcher to Apply initially, Yes

## Build

~~~bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
~~~

That should give you the 'activityfox' executable in the build folder.

## Changelog

### 0.2
- Add Google Chrome and Chromium support
- ActivityFox automatically opens the browser with a profile linked to the current activity id
- ActivityFox gently closes browsers by sending a signal to close the window, mitigating crashes
- Creation of new profiles based on a template profile
- ActivityFox restores browsers in activites they were initially launched

### 0.1
- ActivityFox launches Firefox and terminates it on session saving, restores on session start

## License

GPL v3
Copyright 2012 Yuen Hoe (Jason moofang), 2018 Leonid Kalichkin (hellishnoob)