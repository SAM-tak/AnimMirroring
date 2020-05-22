# UE4 AnimMirroring Plugin

> Read this in other languages: [English](README.md), [日本語](README.ja.md).

![Screenshot](./images/screenshot.png "screenshot")

## Overview

This plug-in is provide "animation mirroring".

## Install

1. Clone the repositry under "{UnrealProjectPath}/Plugins" directory.
2. Right click on the {UnrealProjectPath}/{UnrealProjectName}.uproject" file and select "Generate VisualStudio Project files".
3. Build with generated sln.

## How to use (In case of "Mannequin")

1. Open AnimGraph of AnimBlueprint and add "Mirroring" node.
2. Create an asset of "Mirroring Data" with "Details - Settings" of the added node.
3. Open the added asset and set "Name Rule" to "Tail Match", "Bone Name" to "_l", "Pair Bone Name" to "_r" and "Axis" to "XAxis".
This will invert the bones of the names whose names end with "_l" and "_r".
4. Change "Default Axis" to "XAxis". Now nodes other than pairing mirror on the X axis.
