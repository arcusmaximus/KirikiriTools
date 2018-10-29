#pragma once

class CxdecHelper
{
public:
    static bool         IsCxdecUrl              (const ttstr& url);
    static ttstr        CxdecUrlToXp3FilePath   (const ttstr& url);
    static bool         IsCxdecArchive          (const ttstr& path);
};
