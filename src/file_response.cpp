/*
     This file is part of libhttpserver
     Copyright (C) 2011-2019 Sebastiano Merlino

     This library is free software; you can redistribute it and/or
     modify it under the terms of the GNU Lesser General Public
     License as published by the Free Software Foundation; either
     version 2.1 of the License, or (at your option) any later version.

     This library is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
     Lesser General Public License for more details.

     You should have received a copy of the GNU Lesser General Public
     License along with this library; if not, write to the Free Software
     Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
     USA
*/

#ifdef _WIN32
#include <io.h>

#define _CRT_INTERNAL_NONSTDC_NAMES 1
#define lseek _lseek
#define open _open
#endif

#include "httpserver/file_response.hpp"
#include <fcntl.h>
#include <microhttpd.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifndef _WIN32
#include <unistd.h>
#endif
#include <iosfwd>

#ifdef _WIN32
#if !defined(S_ISREG) && defined(S_IFMT) && defined(S_IFREG)
#define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#endif
#if !defined(S_ISDIR) && defined(S_IFMT) && defined(S_IFDIR)
#define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#endif
#endif

struct MHD_Response;

namespace httpserver {

MHD_Response* file_response::get_raw_response() {
    struct stat sb;

    // Deny everything but regular files
    if (stat(filename.c_str(), &sb) == 0) {
        if (!S_ISREG(sb.st_mode)) return nullptr;
    } else {
        return nullptr;
    }

    int fd = open(filename.c_str(), O_RDONLY);
    if (fd == -1) return nullptr;

    off_t size = lseek(fd, 0, SEEK_END);
    if (size == (off_t) -1) return nullptr;

    if (size) {
        return MHD_create_response_from_fd(size, fd);
    } else {
        return MHD_create_response_from_buffer(0, nullptr, MHD_RESPMEM_PERSISTENT);
    }
}

}  // namespace httpserver
