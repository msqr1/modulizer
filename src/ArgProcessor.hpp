namespace modulizer {

struct Opts {
  bool merge;
  StrView inDir;
  StrView outDir;
  StrView hdrExtRegex;
  StrView srcExtRegex;
  StrView moduleInterfaceExt;
  StrView openExport;
  StrView closeExport;
  Opts();
};

Opts getOptsOrExit(int argc, char* argv[], bool& verbose);

}