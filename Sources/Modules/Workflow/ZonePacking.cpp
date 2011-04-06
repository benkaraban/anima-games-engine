/*
 * Copyright (c) 2010, Anima Games, Benjamin Karaban, Laurent Schneider,
 * Jérémie Comarmond, Didier Colin.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * - Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <Workflow/ZonePacking.h>
#include <Core/LinkedList.h>

#include <algorithm>

namespace Workflow
{
struct ZonePart
{
    int32    x1;
    int32    y1;

    int32    x2;
    int32    y2;
};

struct InternalZone
{
    int32   id;
    int32   width;
    int32   height;
//    int32   maxSize;

    bool operator < (const InternalZone & zone) const
    {
//        return maxSize > zone.maxSize;
        return height > zone.height;
    };
};


bool fit(const ZonePart & part, const InternalZone & zone)
{
    return (zone.width <= (part.x2 - part.x1 + 1)) && (zone.height <= (part.y2 - part.y1 + 1));
}

void internalPackZone(const ZonePart & part, ZonePack & pack, Core::LinkedList<InternalZone> & zones)
{
    Core::LinkedList<InternalZone>::iterator iZone = zones.begin();

    while(iZone != zones.end() && !fit(part, *iZone))
        ++iZone;

    if(iZone != zones.end())
    {
        ZonePack::PackedZone pzone;
        pzone.id = iZone->id;
        pzone.x1 = part.x1;
        pzone.y1 = part.y1;
        pzone.x2 = part.x1 + iZone->width - 1;
        pzone.y2 = part.y1 + iZone->height - 1;

        //   +---------+-------------------------------+
        //   |         |                               |
        //   |   zp    |               p1              |
        //   |         |                               |
        //   +---------+-------------------------------+
        //   |                                         |
        //   |                                         |
        //   |                                         |
        //   |                    p2                   |
        //   |                                         |
        //   |                                         |
        //   |                                         |
        //   +---------+-------------------------------+

        ZonePart p1;
        p1.x1 = pzone.x2 + 1;
        p1.y1 = part.y1;
        p1.x2 = part.x2;
        p1.y2 = pzone.y2;

        ZonePart p2;
        p2.x1 = part.x1;
        p2.y1 = pzone.y2 + 1;
        p2.x2 = part.x2;
        p2.y2 = part.y2;

        pack.zones.push_back(pzone);
        zones.erase(iZone);

        internalPackZone(p1, pack, zones);
        internalPackZone(p2, pack, zones);
    }
}

void packZones(Core::List<ZonePack> & pack, const Core::List<Zone> & zones, int32 maxWidth, int32 maxHeight)
{
    //int32 maxSize = maxPackSize;
    Core::List<InternalZone> internalZones;
    internalZones.resize(zones.size());

    for(int32 ii=0; ii < zones.size(); ii++)
    {
        internalZones[ii].id = ii;
        internalZones[ii].width = zones[ii].width;
        internalZones[ii].height = zones[ii].height;
//        internalZones[ii].maxSize = std::max(zones[ii].width, zones[ii].height);
        maxWidth = std::max(maxWidth, zones[ii].width);
        maxHeight = std::max(maxHeight, zones[ii].height);
    }

    std::sort(internalZones.begin(), internalZones.end());
    Core::LinkedList<InternalZone> sortedZones(internalZones.begin(), internalZones.end());

    ZonePart part;
    part.x1 = 0;
    part.y1 = 0;
    part.x2 = maxWidth - 1;
    part.y2 = maxHeight - 1;

    pack.clear();

    while(!sortedZones.empty())
    {
        ZonePack p;
        p.width = maxWidth;
        p.height = maxHeight;
        internalPackZone(part, p, sortedZones);
        pack.push_back(p);
    }

    for(int32 ii=0; ii < pack.size(); ii++)
    {
        int32 maxX = 0;
        int32 maxY = 0;

        for(int32 jj=0; jj < pack[ii].zones.size(); jj++)
        {
            maxX = std::max(maxX, pack[ii].zones[jj].x2);
            maxY = std::max(maxY, pack[ii].zones[jj].y2);
        }

        pack[ii].width = Core::nextPowerOfTwo(maxX);
        pack[ii].height = Core::nextPowerOfTwo(maxY);
    }
}

}