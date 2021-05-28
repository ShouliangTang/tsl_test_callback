using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Threading;

namespace test_callback
{
    //unsafe public static class common
    //{
    //    public static float R;
    //    public static float* ree;
    //}
    
    unsafe class Program
    {
        //unsafe public static float* ree;
        static float ree;
        unsafe static void Main(string[] args)
        {
            //*RE = 100.0F;
            ree = 100.0F;
            Console.WriteLine("valu = {0:G} ", ree);

            Thread t1 = new Thread(new ThreadStart(link_start));
            t1.Start();
            Thread t2 = new Thread(new ThreadStart(link_get_dif));
            t2.Start();

            //Console.WriteLine("---------- start main ----------");
            //Link.Start();
            //Thread.Sleep(5000);//15S 
            //Console.WriteLine("---------- end main ----------");
            //while (true)
            //{
            //    Console.WriteLine("--------------------");
            //    ree = Link.GetDiff();
            //    Console.WriteLine("addr = 0x{0:x}", (int)ree);////
            //    Console.WriteLine("valu = {0:G} ", *ree);
            //    Thread.Sleep(2000);
            //    Console.WriteLine("--------------------");
            //}

            Console.WriteLine("hdusahdfds");
            Console.ReadLine();

        }

        public static void link_start()
        {
            Console.WriteLine("---------- start main ----------");
            Link.Start();
            Console.WriteLine("---------- end main ----------");
        }
        public static void link_get_dif()
        {
            while (true)
            {
                Console.WriteLine("--------------------");
                ree = Link.GetDiff();
                Console.WriteLine("valu = {0:G} ", ree);
                Thread.Sleep(2000);
                Console.WriteLine("--------------------");
            }
        }
    }
}
