# flcompare
**Files Line Compare**

Compare strings in one (source) file against another (target). Two extra files are output that list source strings that are found and not found.

Uses Boost 1.7.

Files Partial Line Compare args:
        <source_file>
        <compare_file>
        <optional bool: compare as whole words (def. 1)>
        <optional bool: compare is case-sensitive (def. 0)>
        <optional int: progress update throttle (def. 100)>
