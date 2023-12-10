
/**********************************************************************************/
/* wsserver/sha1/sha1.h                                                           */
/*                                                                                */
/* This file contains the details for the sha1 tools of the web socket server     */
/**********************************************************************************/
/*                  This file is part of the ERT-Tiroir project                   */
/*                         github.com/ert-tiroir/wsserver                         */
/**********************************************************************************/
/* This file is 100% in the public domain and can be reused without limitations   */
/*                                                                                */
/* The above and below copyright noticese and this permission notice             */
/* shall be included in all copies or substantial portions of the Software.      */
/*                                                                               */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR    */
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,      */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE   */
/* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER        */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, */
/* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE */
/* SOFTWARE.                                                                     */
/*********************************************************************************/
/* The software originally comes from github.com/vog/sha1                        */
/*                                                                               */
/* Original C Code                                                               */
/*     -- Steve Reid <steve@edmweb.com>                                          */
/* Small changes to fit into bglibs                                              */
/*     -- Bruce Guenter <bruce@untroubled.org>                                   */
/* Translation to simpler C++ Code                                               */
/*     -- Volker Diels-Grabsch <v@njh.eu>                                        */
/* Safety fixes                                                                  */
/*     -- Eugene Hopkinson <slowriot at voxelstorm dot com>                      */
/* Header-only library                                                           */
/*     -- Zlatko Michailov <zlatko@michailov.org>                                */
/* Hex to Base 64                                                                */
/*     -- ERT-Tiroir                                                             */
/*********************************************************************************/

#pragma once

#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

class SHA1
{
public:
    SHA1();
    void update(const std::string &s);
    void update(std::istream &is);
    std::string final();
    std::string final_base64();
    static std::string from_file(const std::string &filename);

private:
    uint32_t digest[5];
    std::string buffer;
    uint64_t transforms;
};
