using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Runtime.InteropServices;

namespace test_callback
{
    class Link
    {

        [DllImport("E:\\jingruishijue\\test\\test_callback\\test_callback_dll\\test_callback_dll\\x64\\Release\\test_callback_dll.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.Cdecl)]
        unsafe public static extern void Start();

        [DllImport("E:\\jingruishijue\\test\\test_callback\\test_callback_dll\\test_callback_dll\\x64\\Release\\test_callback_dll.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.Cdecl)]
        unsafe public static extern float GetDiff();

    }
}
