﻿/*
 *   Gomoku
 *   
 *   A game when you are in the first role, Black Chesses are Forbidden to
 *   Win from three-three, four-four, and six or more in a row. In every
 *   single Suspected Survive Three row, we shall consider any reference
 *   point Forbidden by any subsequent with three-three, four-four, five in a
 *   row, or six or more in a row.
 *   
 *   Copyright (C) 2022  Edmond Chow
 *   
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Affero General Public License as published
 *   by the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *   
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Affero General Public License for more details.
 *   
 *   You should have received a copy of the GNU Affero General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *   
 *   If you have any inquiry, feel free to contact <edmond-chow@outlook.com>.
 */
using System;
using System.Windows.Forms;
namespace Gomoku
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new MainWindow());
        }
    }
}
