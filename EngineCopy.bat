xcopy /s /y  /exclude:exclude_list.txt ".\Project\Engine\Engine\*.h" ".\External\Include\Engine"
xcopy /s /y  /exclude:exclude_list.txt ".\Project\Engine\Engine\*.inl" ".\External\Include\Engine"
xcopy /s /y  /exclude:exclude_list.txt ".\Project\Engine\Engine\*.fx" ".\OutputFile\bin\content\shader"