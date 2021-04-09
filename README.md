# flcompare
**Files Line Compare**

Console program to compare strings in one (source) file against another (target).

Two extra files are output that list source strings that are found and not found in the target.

Source requires Boost 1.7 (untested on other versions).

Command line args:<br>
        <source_file><br>
        <compare_file><br>
        <optional bool: compare as whole words (def. 1)><br>
        <optional bool: compare is case-sensitive (def. 0)><br>
        <optional int: progress update throttle (def. 100)><br>
