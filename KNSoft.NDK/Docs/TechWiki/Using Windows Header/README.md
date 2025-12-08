# Using Windows Header

## Version Control

<table>
    <tr>
        <th rowspan="2">Serial</th>
        <th rowspan="2">Detail</th>
        <th rowspan="2">Codename</th>
        <th rowspan="2">Major</th>
        <th rowspan="2">Minor</th>
        <th rowspan="2">Build Number</th>
        <th rowspan="2">CSD</th>
        <th colspan="2">_WIN32_WINNT</th>
        <th colspan="2">NTDDI_VERSION</th>
    </tr>
    <tr>
        <th>Value</th>
        <th>Name</th>
        <th>Value</th>
        <th>Name</th>
    </tr>
    <tr>
        <td rowspan="5">Windows XP</td>
        <td>-</td>
        <td rowspan="4">Whistler</td>
        <td rowspan="8">5</td>
        <td rowspan="4">1</td>
        <td rowspan="4">2600</td>
        <td>0</td>
        <td rowspan="4">0x0501</td>
        <td rowspan="4">_WIN32_WINNT_WINXP</td>
        <td>0x05010000</td>
        <td>NTDDI_WINXP</td>
    </tr>
    <tr>
        <td>SP1</td>
        <td>1</td>
        <td>0x05010100</td>
        <td>NTDDI_WINXPSP1</td>
    </tr>
    <tr>
        <td>SP2</td>
        <td>2</td>
        <td>0x05010200</td>
        <td>NTDDI_WINXPSP2</td>
    </tr>
    <tr>
        <td>SP3</td>
        <td>3</td>
        <td>0x05010300</td>
        <td>NTDDI_WINXPSP3</td>
    </tr>
    <tr>
        <td>x64</td>
        <td>Anvil</td>
        <td rowspan="4">2</td>
        <td rowspan="4">3790</td>
        <td>?</td>
        <td rowspan="4">0x0502</td>
        <td>-</td>
        <td rowspan="2">0x05020000</td>
        <td>-</td>
    </tr>
    <tr>
        <td rowspan="3">Windows Server 2003</td>
        <td>-</td>
        <td rowspan="3">Whistler Server</td>
        <td>0</td>
        <td rowspan="3">_WIN32_WINNT_WS03</td>
        <td>NTDDI_WS03</td>
    </tr>
    <tr>
        <td>SP1</td>
        <td>1</td>
        <td>0x05020100</td>
        <td>NTDDI_WS03SP1</td>
    </tr>
    <tr>
        <td>SP2</td>
        <td>2</td>
        <td>0x05020200</td>
        <td>NTDDI_WS03SP2</td>
    </tr>
    <tr>
        <td rowspan="5">Windows Vista</td>
        <td>-</td>
        <td rowspan="5">Longhorn</td>
        <td rowspan="13">6</td>
        <td rowspan="5">0</td>
        <td>6000</td>
        <td>0</td>
        <td rowspan="5">0x0600</td>
        <td rowspan="5">_WIN32_WINNT_LONGHORN<br>_WIN32_WINNT_WIN6<br>_WIN32_WINNT_VISTA<br>_WIN32_WINNT_WS08</td>
        <td>0x06000000</td>
        <td>NTDDI_LONGHORN<br>NTDDI_WIN6<br>NTDDI_VISTA</td>
    </tr>
    <tr>
        <td>SP1</td>
        <td rowspan="2">6001</td>
        <td>1</td>
        <td rowspan="2">0x06000100</td>
        <td rowspan="2">NTDDI_WIN6SP1<br>NTDDI_VISTASP1<br>NTDDI_WS08</td>
    </tr>
    <tr>
        <td>Server 2008</td>
        <td>?</td>
    </tr>
    <tr>
        <td>SP2</td>
        <td rowspan="2">6002</td>
        <td rowspan="2">2</td>
        <td rowspan="2">0x06000200</td>
        <td rowspan="2">NTDDI_WIN6SP2<br>NTDDI_VISTASP2<br>NTDDI_WS08SP2</td>
    </tr>
    <tr>
        <td>Server 2008 SP2</td>
    </tr>
    <tr>
        <td rowspan="4">Windows 7</td>
        <td>-</td>
        <td rowspan="4">Windows 7</td>
        <td rowspan="4">1</td>
        <td rowspan="2">7600</td>
        <td rowspan="2">0</td>
        <td rowspan="4">0x0601</td>
        <td rowspan="4">_WIN32_WINNT_WIN7</td>
        <td rowspan="4">0x06010000</td>
        <td rowspan="4">NTDDI_WIN7</td>
    </tr>
    <tr>
        <td>Server 2008 R2</td>
    </tr>
    <tr>
        <td>-</td>
        <td rowspan="2">7601</td>
        <td rowspan="2">1</td>
    </tr>
    <tr>
        <td>Server 2008 R2 SP1</td>
    </tr>
    <tr>
        <td rowspan="2">Windows 8</td>
        <td>-</td>
        <td>Windows 8</td>
        <td rowspan="2">2</td>
        <td rowspan="2">9200</td>
        <td rowspan="22">0</td>
        <td rowspan="2">0x0602</td>
        <td rowspan="2">_WIN32_WINNT_WIN8</td>
        <td rowspan="2">0x06020000</td>
        <td rowspan="2">NTDDI_WIN8</td>
    </tr>
    <tr>
        <td>Server 2012</td>
        <td>Windows Server "8"</td>
    </tr>
    <tr>
        <td rowspan="2">Windows 8.1</td>
        <td>-</td>
        <td rowspan="2">Blue</td>
        <td rowspan="2">3</td>
        <td rowspan="2">9600</td>
        <td rowspan="2">0x0603</td>
        <td rowspan="2">_WIN32_WINNT_WINBLUE</td>
        <td rowspan="2">0x06030000</td>
        <td rowspan="2">NTDDI_WINBLUE</td>
    </tr>
    <tr>
        <td>Server 2012 R2</td>
    </tr>
    <tr>
        <td rowspan="15">Windows 10</td>
        <td>1507</td>
        <td>Threshold</td>
        <td rowspan="20">10</td>
        <td rowspan="20">0</td>
        <td>10240</td>
        <td rowspan="20">0x0A00</td>
        <td rowspan="2">_WIN32_WINNT_WINTHRESHOLD</td>
        <td>0x0A000000</td>
        <td>NTDDI_WIN10<br>NTDDI_WINTHRESHOLD</td>
    </tr>
    <tr>
        <td>1511</td>
        <td>Threshold 2</td>
        <td>10586</td>
        <td>0x0A000001</td>
        <td>NTDDI_WIN10_TH2</td>
    </tr>
    <tr>
        <td>1607</td>
        <td>Redstone</td>
        <td>14393</td>
        <td rowspan="19">_WIN32_WINNT_WIN10</td>
        <td>0x0A000002</td>
        <td>NTDDI_WIN10_RS1</td>
    </tr>
    <tr>
        <td>1703</td>
        <td>Redstone 2</td>
        <td>15063</td>
        <td>0x0A000003</td>
        <td>NTDDI_WIN10_RS2</td>
    </tr>
    <tr>
        <td>1709</td>
        <td>Redstone 3</td>
        <td>16299</td>
        <td>0x0A000004</td>
        <td>NTDDI_WIN10_RS3</td>
    </tr>
    <tr>
        <td>1803</td>
        <td>Redstone 4</td>
        <td>17134</td>
        <td>0x0A000005</td>
        <td>NTDDI_WIN10_RS4</td>
    </tr>
    <tr>
        <td>1809</td>
        <td>Redstone 5</td>
        <td>17763</td>
        <td>0x0A000006</td>
        <td>NTDDI_WIN10_RS5</td>
    </tr>
    <tr>
        <td>1903</td>
        <td>19H1</td>
        <td>18362</td>
        <td>0x0A000007</td>
        <td>NTDDI_WIN10_19H1</td>
    </tr>
    <tr>
        <td>1909 19H2</td>
        <td>Vanadium</td>
        <td>18363</td>
        <td>0x0A000008</td>
        <td>NTDDI_WIN10_VB</td>
    </tr>
    <tr>
        <td>2004 20H1</td>
        <td rowspan="6">Vibranium</td>
        <td>19041</td>
        <td>0x0A000009</td>
        <td>NTDDI_WIN10_MN</td>
    </tr>
    <tr>
        <td>20H2</td>
        <td>19042</td>
        <td>0x0A00000A</td>
        <td>NTDDI_WIN10_FE</td>
    </tr>
    <tr>
        <td>21H1</td>
        <td>19043</td>
        <td></td>
        <td></td>
    </tr>
    <tr>
        <td>21H2</td>
        <td>19044</td>
        <td>0x0A00000B</td>
        <td>NTDDI_WIN10_CO</td>
    </tr>
    <tr>
        <td>22H2</td>
        <td>19045</td>
        <td>0x0A00000C</td>
        <td>NTDDI_WIN10_NI</td>
    </tr>
    <tr>
        <td></td>
        <td></td>
        <td>0x0A00000D</td>
        <td>NTDDI_WIN10_CU</td>
    </tr>
    <tr>
        <td rowspan="5">Windows 11</td>
        <td>21H2</td>
        <td>Cobalt, Sun Valley</td>
        <td>22000</td>
        <td>0x0A00000E</td>
        <td>NTDDI_WIN11_ZN</td>
    </tr>
    <tr>
        <td>22H2</td>
        <td>Nickel, Sun Valley 2</td>
        <td>22621</td>
        <td>0x0A00000F</td>
        <td>NTDDI_WIN11_GA</td>
    </tr>
    <tr>
        <td>23H2</td>
        <td>Nickel, Sun Valley 3</td>
        <td>22631</td>
        <td></td>
        <td></td>
    </tr>
    <tr>
        <td>24H2</td>
        <td>Germanium, Hudson Valley</td>
        <td>26100</td>
        <td rowspan="2">0x0A000010</td>
        <td rowspan="2">NTDDI_WIN11_GE</td>
    </tr>
    <tr>
        <td>25H2</td>
        <td></td>
        <td>26200</td>
    </tr>
</table>
