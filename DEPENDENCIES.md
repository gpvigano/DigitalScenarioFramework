### Dependencies

This framework uses [boost] (version 1.70.0, filesystem, date_time and dll libraries) and [rapidjson] (version 1.1.0) libraries.
This project uses some libraries (`gpvulc_text`, `gpvulc_path`, `gpvulc_time`, `gpvulc_json`, `gpvulc_console`) developed in another project, [gpvulc].

A subset of the external libraries needed to build this project can be found in the root folder of this repository, packed in a zip archive (`deps.zip`), you can unpack this archive in the root repository folder. To build each library included you will find [Visual Studio] and [Code::Blocks] projects in a 'projects' subfolder. A [Visual Studio] solution and a [Code::Blocks] workspace are available in the `deps` folder to build all the dependencies.

If you want to replace them you have two options.
1. Replace in this folder the external libraries:
  - include files of [rapidjson] must be put in `depend/rapidjson/` (e.g. `depend/rapidjson/rapidjson/`).
  - for [boost] libraries include files must be put in `depend/boost/` (e.g. `depend/boost/boost/`) and compiled binaries must be put in `depend/boost/lib/` folder.
  - For [gpvulc] libraries you must put it in `depend/gpvulc/` (e.g. `depend/gpvulc/include/gpvulc/`, `depend/gpvulc/lib/`) folder.
2. Change this configuration by editing the property sheet [`DSF_external.props`](./DSF_deps.props) in the `DigitalScenarioFramework` folder to specify the paths to the external libraries:
  - change the `DEPEND_ROOT` variable, if needed;
  - set the `GPVULC_ROOT` variable;
  - set the `RAPIDJSON_ROOT` variable;
  - set the `BOOST_ROOT` variable;
  - change also the `BOOST_VER` variable, if needed.


[boost]: https://www.boost.org/
[rapidjson]: https://github.com/miloyip/rapidjson/
[gpvulc]: https://github.com/gpvigano/gpvulc

[Visual Studio]: https://visualstudio.microsoft.com/
[Code::Blocks]: https://www.codeblocks.org/
