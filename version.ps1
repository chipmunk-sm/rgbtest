    #powershell.exe -noprofile -executionpolicy bypass -file version.ps1

    $Build = $env:APPVEYOR_BUILD_NUMBER
    $distrver = $env:APPVEYOR_JOB_NAME
    $appBranch= $env:APPVEYOR_REPO_BRANCH
    #$appBranch= '"{0}"' -f $env:APPVEYOR_REPO_BRANCH
    $buildFolder = $env:APPVEYOR_BUILD_FOLDER
    $sourceDir= $env:APPVEYOR_BUILD_FOLDER
    
    if (!$Build)
    {
        $Build = 0
    }

    if (!$distrver)
    {
        $distrver = (get-itemproperty -Path "HKLM:\SOFTWARE\Microsoft\Windows NT\CurrentVersion" -Name ProductName).ProductName
        if (!$distrver)
        {
            $distribution = Invoke-Expression "lsb_release -cs"
            $version = Invoke-Expression "lsb_release -rs"
            $distrver = "$distribution.$version"
        }
    }

    if (!$appBranch)
    {
        $appBranch = $(git rev-parse --abbrev-ref HEAD)
    }

    if (!$buildFolder)
    {
        $buildFolder = Get-Location
    }

    if (!$sourceDir)
    {
        $sourceDir = Get-Location
    }

    Write-Host "-Build [$Build]`n"
    Write-Host "-distrver [$distrver]`n"
    Write-Host "-appBranch [$appBranch]`n"
    Write-Host "-buildFolder [$buildFolder]`n"
    Write-Host "-sourceDir [$sourceDir]`n"

    $revision = "HEAD" 
    Write-Host "*** Start 'get version info from tag'`n"
    Write-Host "Expected tag 'v1.0-release_name' or '1.0-release_name'`n"
    $LastTag = $(git -C $sourceDir describe --tags --first-parent --match "*" $revision)
    if (!$LastTag) 
    {
        $LastTag = "0.0-not_set"
        Write-Host "Failed on get tag for revision $revision - defaulting to [$LastTag]`n"
    }
    $delim = ".",";","-"
    $srrayStr = @($LastTag -Split {$delim -contains $_})
    $Major = $srrayStr[0] -replace '\D+(\d+)','$1'
    $Minor = $srrayStr[1]
    $buildName = $srrayStr[2]
    if (Get-Command Update-AppveyorBuild -errorAction SilentlyContinue)
    {
        Update-AppveyorBuild -Version "$Major.$Minor.$Build.$appBranch"
    }
    $env:APPVEYOR_BUILD_VERSION="$Major.$Minor.$Build.$appBranch"
    Write-Host "*** End 'get version info from tag'`n"

    
    Write-Host "*** Start 'create ver.h'`n"
    $verContents = "`n#define FVER1 $Major`n#define FVER2 $Minor`n#define FVER3 $Build`n#define FVER4 0`n#define FVER_NAME `"$buildName`"`n#define FBRANCH `"$appBranch`"`n#define FDISTR `"$distrver`"`n"
    $verFile = $(Join-Path $buildFolder "ver.h")
    [System.IO.File]::WriteAllText($verFile, $verContents)
    Write-Host "Create ver.h [$verFile]`n$(Get-Content $verFile -Raw -Encoding Ascii -ErrorAction Ignore)"  
    Write-Host "`n*** End 'create ver.h'`n"
    
    
    Write-Host "*** Start 'create recipe'`n"
    $recipeFile = $(Join-Path $buildFolder "tmp")
    New-Item -ItemType directory -Force -Path $recipeFile > $null
    $recipeFile = $(Join-Path $recipeFile "recipe.txt")
    #{latest-tag}~0~{revtime}
    #~0~$appBranch.$buildName
    $newRecipeContents = "#git-build-recipe format 0.4 deb-version $Major.$Minor.$Build`nlp:rgbtest $appBranch`n"
    [System.IO.File]::WriteAllText($recipeFile, $newRecipeContents)
    Write-Host "Create recipe [$recipeFile]`n$(Get-Content $recipeFile -Raw -Encoding Ascii -ErrorAction Ignore)" 
    Write-Host "*** End 'create recipe'`n"
    
    
    Write-Host "*** Start 'create release note'"
    $gitTagList = $(git -C $sourceDir tag --sort=-version:refname)
    if(1 -ge $gitTagList.Count){
        Write-Host "Use all entries for a release note"
        $releaseNoteContents = git -C $sourceDir log  --date=short --pretty=format:"  * %ad [%aN] %s"
     }else{
        $gitTagRange = $gitTagList[1]
        $gitTagRange += "..$revision"
        #$gitTagRange += ".." + $gitTagList[0]
        Write-Host "Release note range [$gitTagRange]"
        $releaseNoteContents = git -C $sourceDir log "$gitTagRange" --date=short --pretty=format:"  * %ad [%aN] %s"
    }

    $releaseNoteFile = $(Join-Path $buildFolder releaseNote.txt)
    $releaseNoteContents | Out-File $releaseNoteFile
    Write-Host "Create release note [$releaseNoteFile]`n"
    #$(Get-Content $releaseNoteFile -Raw -Encoding Ascii -ErrorAction Ignore)  
    Write-Host "*** End 'create release note'`n"
    
    
    Write-Host "*** Start 'update debian changelog'"
    $debianChangelogFile = $(Join-Path $buildFolder debian/changelog)
    $Time = $(Get-Date -UFormat "%a, %d %b %Y %T %Z00")

    "rgbtest ($Major.$Minor.$Build) unstable; urgency=medium`n" | Out-File $debianChangelogFile
    Add-Content -Path $debianChangelogFile -Value $releaseNoteContents
    Add-Content -Path $debianChangelogFile -Value "`n -- chipmunk.sm <dannico@linuxmail.org>  $Time"

    Write-Host "Create debian/changelog [$debianChangelogFile]`n$(Get-Content $debianChangelogFile -Raw -Encoding Ascii -ErrorAction Ignore)" 
    Write-Host "*** End 'update debian changelog'`n"
    
    
