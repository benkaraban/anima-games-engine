# -*- coding: utf8 -*-

import os
import zipfile
import shutil

# Ce script a pour but de générer l'exécutable ChronoRage.exe
# et de créer un fichier .pak contenant tous les assets requis

chronorage_vfs_path = '..\\..\\Data'
chronorage_data_path = os.path.join(chronorage_vfs_path, 'ChronoRageData')
chronorage_binaries_dir = '..\\..\\Build\\VS2008_Shared\\OutputExe\\Release'
chronorage_shaders_path = '..\\..\\Shaders'
chronorage_installs_path = '..\\..\\LibsExternes\\Installs'
chronorage_installer_dir = '..\\..\\Build\\VS2008_Shared\\'

chronorage_dest_dir = '..\\..\\Build\\VS2008_Shared\\OutputExe\\Release\\ChronoRage'
chronorage_demo_dest_dir = '..\\..\\Build\\VS2008_Shared\\OutputExe\\Release\\ChronoRageDemo'

chronorage_installer_name = 'ChronoRage_press_preview.exe'
chronorage_demo_installer_name = 'ChronoRage_demo.exe'

def copy_erase(src, dst):
    print 'Copying ' + src
    shutil.copy(src, dst)
    
def copy_tree(src, dst):
    print 'Copying ' + src + '\\'
    if(os.path.exists(dst)):
        shutil.rmtree(dst)
    shutil.copytree(src, dst, False, shutil.ignore_patterns('*.svn'))
    
    
class PackChronoRageData:
    def __init__(self):
        self.files = []
        
        self.dataAuthExt = []
        self.dataAuthExt.append('.tga')
        self.dataAuthExt.append('.tex')
        self.dataAuthExt.append('.pbk')
        self.dataAuthExt.append('.group')
        self.dataAuthExt.append('.wav')
        self.dataAuthExt.append('.ogg')
        self.dataAuthExt.append('.world')
        self.dataAuthExt.append('.font.xml')
        
        self.dataDir = []
        self.dataDir.append('.')
        self.dataDir.append('Explosions')
        self.dataDir.append('Foes')
        self.dataDir.append('Foes\\Shots')
        self.dataDir.append('Font')
        self.dataDir.append('Musics')
        self.dataDir.append('Sounds')
        
        self.excludeFiles = []
        self.excludeFiles.append('DemoGUI.pbk')
        self.excludeFiles.append('waves-demo.xml')
        
        self.dataZipLoc = 'ChronoRageData.pak'
        self.dataZip = zipfile.ZipFile(self.dataZipLoc, 'w', zipfile.ZIP_STORED, False)
        
        self.dictZipLoc = 'ChronoRageDict.pak'
        self.dictZip = zipfile.ZipFile(self.dictZipLoc, 'w', zipfile.ZIP_STORED, False)
        
        self.levelsDir = []
        self.levelsDir.append('.')
        self.levelsDir.append('.\\Waves')
        self.levelsZipLoc = 'ChronoRageLevels.pak'
        self.levelsZip = zipfile.ZipFile(self.levelsZipLoc, 'w', zipfile.ZIP_STORED, False)
        
        self.shadersZipLoc = 'ChronoRageShaders.pak'
        self.shadersZip = zipfile.ZipFile(self.shadersZipLoc, 'w', zipfile.ZIP_STORED, False)
        
    
    def packChronoRageData(self):
        print '------------------------------------------------------------'
        print '              Packing Chrono Rage Data                      '
        print '------------------------------------------------------------'
        back_dir = os.getcwd()
        os.chdir(chronorage_data_path)
        
        for dir in self.dataDir:
            files = os.listdir(dir)
            for file in files:
                for ext in self.dataAuthExt:
                    if file.endswith(ext) and self.excludeFiles.count(file) == 0:
                        fullName = os.path.join(dir, file)
                        print fullName
                        self.dataZip.write(fullName)
        
        #A ce niveau le fichier zip est prêt il ne reste plus qu'à le copier dans le répertoire désiré
        os.chdir(back_dir)
        
        self.dataZip.close()
        if not os.path.exists(chronorage_dest_dir):
            os.makedirs(chronorage_dest_dir)
        
        if os.path.exists(os.path.join(chronorage_dest_dir, self.dataZipLoc)):
            os.remove(os.path.join(chronorage_dest_dir, self.dataZipLoc))
            
        shutil.move(self.dataZipLoc, chronorage_dest_dir)
    
    def packChronoRageLevels(self):
        print '------------------------------------------------------------'
        print '              Packing Chrono Rage Levels                    '
        print '------------------------------------------------------------'
        back_dir = os.getcwd()
        os.chdir(os.path.join(chronorage_data_path, 'Levels'))
        
        levelsAuthExt = []
        levelsAuthExt.append('.xml')
        levelsAuthExt.append('.world')
        
        forbiddenExt = '.world.xml'
        
        for dir in self.levelsDir:
            files = os.listdir(dir)
            for file in files:
                for ext in levelsAuthExt:
                    if file.endswith(ext) and not file.endswith(forbiddenExt) and self.excludeFiles.count(file) == 0:
                        fullName = os.path.join(dir, file)
                        print fullName
                        self.levelsZip.write(fullName)
        
        #A ce niveau le fichier zip est prêt il ne reste plus qu'à le copier dans le répertoire désiré
        os.chdir(back_dir)
        
        self.levelsZip.close()
        if not os.path.exists(chronorage_dest_dir):
            os.makedirs(chronorage_dest_dir)
        
        if os.path.exists(os.path.join(chronorage_dest_dir, self.levelsZipLoc)):
            os.remove(os.path.join(chronorage_dest_dir, self.levelsZipLoc))
            
        shutil.move(self.levelsZipLoc, chronorage_dest_dir)
    
    def packChronoRageDict(self):
        print '------------------------------------------------------------'
        print '              Packing Chrono Rage Dictionaries              '
        print '------------------------------------------------------------'
        back_dir = os.getcwd()
        dir = os.path.join(chronorage_data_path, 'Dictionaries')
        os.chdir(dir)
        
        files = os.listdir('.')
        for file in files:
            if file.endswith('.xml') and self.excludeFiles.count(file) == 0:
                print file
                self.dictZip.write(file)
        
        #A ce niveau le fichier zip est prêt il ne reste plus qu'à le copier dans le répertoire désiré
        os.chdir(back_dir)
        
        self.dictZip.close()
        if not os.path.exists(chronorage_dest_dir):
            os.makedirs(chronorage_dest_dir)
        
        if os.path.exists(os.path.join(chronorage_dest_dir, self.dictZipLoc)):
            os.remove(os.path.join(chronorage_dest_dir, self.dictZipLoc))
            
        shutil.move(self.dictZipLoc, chronorage_dest_dir)
    
    def packChronoRageShaders(self):
        print '------------------------------------------------------------'
        print '              Packing Chrono Rage Shaders                   '
        print '------------------------------------------------------------'
        back_dir = os.getcwd()
        os.chdir(chronorage_shaders_path)
        dir = 'bin'
        files = os.listdir(dir)
        for file in files:
            if file.endswith('.obj') and self.excludeFiles.count(file) == 0:
                print file
                self.shadersZip.write(os.path.join(dir, file))
        
        #A ce niveau le fichier zip est prêt il ne reste plus qu'à le copier dans le répertoire désiré
        os.chdir(back_dir)
        
        self.shadersZip.close()
        if not os.path.exists(chronorage_dest_dir):
            os.makedirs(chronorage_dest_dir)
        
        if os.path.exists(os.path.join(chronorage_dest_dir, self.shadersZipLoc)):
            os.remove(os.path.join(chronorage_dest_dir, self.shadersZipLoc))
            
        shutil.move(self.shadersZipLoc, chronorage_dest_dir)
        
    def copyBinaries(self):
        print '------------------------------------------------------------'
        print '                      Copy Binaries                         '
        print '------------------------------------------------------------'
        binaries = []
        binaries.append('ChronoRage.exe')
        binaries.append('Assets.dll')
        binaries.append('Audio.dll')
        binaries.append('AudioOpenAL.dll')
        binaries.append('boost_date_time-vc90-mt-1_40.dll')
        binaries.append('Core.dll')
        binaries.append('Gfx.dll')
        binaries.append('GfxDx9.dll')
        binaries.append('GUI.dll')
        binaries.append('lua52.dll')
        binaries.append('ogg.dll')
        binaries.append('Renderer.dll')
        binaries.append('RendererSM2.dll')
        binaries.append('Universe.dll')
        binaries.append('vorbis.dll')
        binaries.append('vorbisfile.dll')
        binaries.append('Window.dll')
        
        back_dir = os.getcwd()
        os.chdir(chronorage_binaries_dir)
        
        for file in binaries:
            fullPath = os.path.join('.\\ChronoRage', file)
            if os.path.exists(fullPath):
                os.remove(fullPath)
            print(file)
            shutil.copy(file, '.\\ChronoRage')
    
        os.chdir(back_dir)
        
    def copyVFS(self):
        print '------------------------------------------------------------'
        print '                      Copy VFS                              '
        print '------------------------------------------------------------'
        
        back_dir = os.getcwd()
        os.chdir(chronorage_vfs_path)
        
        shutil.copy('VFS-ChronoRage_release.xml', os.path.join(back_dir, 'VFS-ChronoRage.xml'))
        os.chdir(back_dir)
        if os.path.exists(os.path.join(chronorage_dest_dir, 'VFS-ChronoRage.xml')):
            os.remove(os.path.join(chronorage_dest_dir, 'VFS-ChronoRage.xml'))
            
        shutil.move('VFS-ChronoRage.xml', chronorage_dest_dir)
        print('VFS-ChronoRage.xml_release -> VFS-ChronoRage.xml')
        
    
    def copyDependencyInstalls(self):
        print '------------------------------------------------------------'
        print '             Copy Dependency Installs                       '
        print '------------------------------------------------------------'
        
        copy_erase(os.path.join(chronorage_installs_path, 'oalinst.exe'), chronorage_dest_dir)
        copy_erase(os.path.join(chronorage_installs_path, 'vcredist_x86.exe'), chronorage_dest_dir)
        copy_tree(os.path.join(chronorage_installs_path, 'dxsetup'), os.path.join(chronorage_dest_dir, 'dxsetup'))
        
    def makeInstaller(self):
        print '------------------------------------------------------------'
        print '                   Make Installer                           '
        print '------------------------------------------------------------'
        
        cmd = '"C:\\Program Files (x86)\\NSIS\\makensis.exe" _makeInstall.nsi'
        os.system(cmd)
        
        if os.path.exists(os.path.join(chronorage_installer_dir, chronorage_installer_name)):
            os.remove(os.path.join(chronorage_installer_dir, chronorage_installer_name))
            
        shutil.move(chronorage_installer_name, chronorage_installer_dir)
        
        print("")
        print(chronorage_installer_dir + chronorage_installer_name + " succesfully created.")
    
class PackChronoRageDemoData:
    def __init__(self):
        self.files = []
        
        self.dataAuthExt = []
        self.dataAuthExt.append('.tga')
        self.dataAuthExt.append('.tex')
        self.dataAuthExt.append('.pbk')
        self.dataAuthExt.append('.group')
        self.dataAuthExt.append('.wav')
        self.dataAuthExt.append('.ogg')
        self.dataAuthExt.append('.world')
        self.dataAuthExt.append('.font.xml')
        
        self.dataDir = []
        self.dataDir.append('.')
        self.dataDir.append('Explosions')
        self.dataDir.append('Foes')
        self.dataDir.append('Foes\\Shots')
        self.dataDir.append('Font')
        self.dataDir.append('Musics')
        self.dataDir.append('Musics\\Demo')
        self.dataDir.append('Sounds')
        
        self.excludeFiles = []
        self.excludeFiles.append('01_adrenaline_full_loop.ogg')
        self.excludeFiles.append('02_afterburner_60_loop.ogg')
        self.excludeFiles.append('03_hyperdrive_full.ogg')
        self.excludeFiles.append('04_BrandingPower_BP42-Starflight-Steele.ogg')
        self.excludeFiles.append('05_DigitalAge_DA04-ElectroCity-Steele.ogg')
        self.excludeFiles.append('06_downhill_full.ogg')
        self.excludeFiles.append('07_on_the_move_full.ogg')
        
        self.excludeFileDir = 'Musics'
        
        self.dataZipLoc = 'ChronoRageData.pak'
        self.dataZip = zipfile.ZipFile(self.dataZipLoc, 'w', zipfile.ZIP_STORED, False)
        
        self.dictZipLoc = 'ChronoRageDict.pak'
        self.dictZip = zipfile.ZipFile(self.dictZipLoc, 'w', zipfile.ZIP_STORED, False)
        
        self.levelsDir = []
        self.levelsDir.append('.')
        self.levelsDir.append('.\\Waves')
        self.levelsZipLoc = 'ChronoRageLevels.pak'
        self.levelsZip = zipfile.ZipFile(self.levelsZipLoc, 'w', zipfile.ZIP_STORED, False)
        
        self.shadersZipLoc = 'ChronoRageShaders.pak'
        self.shadersZip = zipfile.ZipFile(self.shadersZipLoc, 'w', zipfile.ZIP_STORED, False)
        
    
    def packChronoRageData(self):
        print '------------------------------------------------------------'
        print '            Packing Chrono Rage Demo Data                   '
        print '------------------------------------------------------------'
        back_dir = os.getcwd()
        os.chdir(chronorage_data_path)
        
        for dir in self.dataDir:
            files = os.listdir(dir)
            for file in files:
                for ext in self.dataAuthExt:
                    if file.endswith(ext) and (dir != self.excludeFileDir or self.excludeFiles.count(file) == 0):
                        fullName = os.path.join(dir, file)
                        print fullName
                        self.dataZip.write(fullName)
        
        #A ce niveau le fichier zip est prêt il ne reste plus qu'à le copier dans le répertoire désiré
        os.chdir(back_dir)
        
        self.dataZip.close()
        if not os.path.exists(chronorage_demo_dest_dir):
            os.makedirs(chronorage_demo_dest_dir)
        
        if os.path.exists(os.path.join(chronorage_demo_dest_dir, self.dataZipLoc)):
            os.remove(os.path.join(chronorage_demo_dest_dir, self.dataZipLoc))
            
        shutil.move(self.dataZipLoc, chronorage_demo_dest_dir)
    
    def packChronoRageLevels(self):
        print '------------------------------------------------------------'
        print '             Packing Chrono Rage Demo Levels                '
        print '------------------------------------------------------------'
        back_dir = os.getcwd()
        os.chdir(os.path.join(chronorage_data_path, 'Levels'))
        
        levelsAuthExt = []
        levelsAuthExt.append('.xml')
        levelsAuthExt.append('.world')
        
        forbiddenExt = '.world.xml'
        
        for dir in self.levelsDir:
            files = os.listdir(dir)
            for file in files:
                for ext in levelsAuthExt:
                    if file.endswith(ext) and not file.endswith(forbiddenExt):
                        fullName = os.path.join(dir, file)
                        print fullName
                        self.levelsZip.write(fullName)
        
        #A ce niveau le fichier zip est prêt il ne reste plus qu'à le copier dans le répertoire désiré
        os.chdir(back_dir)
        
        self.levelsZip.close()
        if not os.path.exists(chronorage_demo_dest_dir):
            os.makedirs(chronorage_demo_dest_dir)
        
        if os.path.exists(os.path.join(chronorage_demo_dest_dir, self.levelsZipLoc)):
            os.remove(os.path.join(chronorage_demo_dest_dir, self.levelsZipLoc))
            
        shutil.move(self.levelsZipLoc, chronorage_demo_dest_dir)
    
    def packChronoRageDict(self):
        print '------------------------------------------------------------'
        print '              Packing Chrono Rage Demo Dictionaries         '
        print '------------------------------------------------------------'
        back_dir = os.getcwd()
        dir = os.path.join(chronorage_data_path, 'Dictionaries')
        os.chdir(dir)
        
        files = os.listdir('.')
        for file in files:
            if file.endswith('.xml'):
                print file
                self.dictZip.write(file)
        
        #A ce niveau le fichier zip est prêt il ne reste plus qu'à le copier dans le répertoire désiré
        os.chdir(back_dir)
        
        self.dictZip.close()
        if not os.path.exists(chronorage_demo_dest_dir):
            os.makedirs(chronorage_demo_dest_dir)
        
        if os.path.exists(os.path.join(chronorage_demo_dest_dir, self.dictZipLoc)):
            os.remove(os.path.join(chronorage_demo_dest_dir, self.dictZipLoc))
            
        shutil.move(self.dictZipLoc, chronorage_demo_dest_dir)
    
    def packChronoRageShaders(self):
        print '------------------------------------------------------------'
        print '              Packing Chrono Rage Demo Shaders              '
        print '------------------------------------------------------------'
        back_dir = os.getcwd()
        os.chdir(chronorage_shaders_path)
        dir = 'bin'
        files = os.listdir(dir)
        for file in files:
            if file.endswith('.obj'):
                print file
                self.shadersZip.write(os.path.join(dir, file))
        
        #A ce niveau le fichier zip est prêt il ne reste plus qu'à le copier dans le répertoire désiré
        os.chdir(back_dir)
        
        self.shadersZip.close()
        if not os.path.exists(chronorage_demo_dest_dir):
            os.makedirs(chronorage_demo_dest_dir)
        
        if os.path.exists(os.path.join(chronorage_demo_dest_dir, self.shadersZipLoc)):
            os.remove(os.path.join(chronorage_demo_dest_dir, self.shadersZipLoc))
            
        shutil.move(self.shadersZipLoc, chronorage_demo_dest_dir)
        
    def copyBinaries(self):
        print '------------------------------------------------------------'
        print '                      Copy Demo Binaries                    '
        print '------------------------------------------------------------'
        binaries = []
        binaries.append('ChronoRageDemo.exe')
        binaries.append('Assets.dll')
        binaries.append('Audio.dll')
        binaries.append('AudioOpenAL.dll')
        binaries.append('boost_date_time-vc90-mt-1_40.dll')
        binaries.append('Core.dll')
        binaries.append('Gfx.dll')
        binaries.append('GfxDx9.dll')
        binaries.append('GUI.dll')
        binaries.append('lua52.dll')
        binaries.append('ogg.dll')
        binaries.append('Renderer.dll')
        binaries.append('RendererSM2.dll')
        binaries.append('Universe.dll')
        binaries.append('vorbis.dll')
        binaries.append('vorbisfile.dll')
        binaries.append('Window.dll')
        
        back_dir = os.getcwd()
        os.chdir(chronorage_binaries_dir)
        
        for file in binaries:
            fullPath = os.path.join('.\\ChronoRageDemo', file)
            if os.path.exists(fullPath):
                os.remove(fullPath)
            print(file)
            shutil.copy(file, '.\\ChronoRageDemo')
    
        os.chdir(back_dir)
        
    def copyVFS(self):
        print '------------------------------------------------------------'
        print '                      Copy Demo VFS                         '
        print '------------------------------------------------------------'
        
        back_dir = os.getcwd()
        os.chdir(chronorage_vfs_path)
        
        shutil.copy('VFS-ChronoRage_release.xml', os.path.join(back_dir, 'VFS-ChronoRage.xml'))
        os.chdir(back_dir)
        if os.path.exists(os.path.join(chronorage_demo_dest_dir, 'VFS-ChronoRage.xml')):
            os.remove(os.path.join(chronorage_demo_dest_dir, 'VFS-ChronoRage.xml'))
            
        shutil.move('VFS-ChronoRage.xml', chronorage_demo_dest_dir)
        print('VFS-ChronoRage.xml_release -> VFS-ChronoRage.xml')
        
    
    def copyDependencyInstalls(self):
        print '------------------------------------------------------------'
        print '             Copy Demo Dependency Installs                  '
        print '------------------------------------------------------------'
        
        copy_erase(os.path.join(chronorage_installs_path, 'oalinst.exe'), chronorage_demo_dest_dir)
        copy_erase(os.path.join(chronorage_installs_path, 'vcredist_x86.exe'), chronorage_demo_dest_dir)
        copy_tree(os.path.join(chronorage_installs_path, 'dxsetup'), os.path.join(chronorage_demo_dest_dir, 'dxsetup'))
        
    def makeInstaller(self):
        print '------------------------------------------------------------'
        print '                   Make Demo Installer                      '
        print '------------------------------------------------------------'
        
        cmd = '"C:\\Program Files (x86)\\NSIS\\makensis.exe" _makeDemoInstall.nsi'
        os.system(cmd)
        
        if os.path.exists(os.path.join(chronorage_installer_dir, chronorage_demo_installer_name)):
            os.remove(os.path.join(chronorage_installer_dir, chronorage_demo_installer_name))
            
        shutil.move(chronorage_demo_installer_name, chronorage_installer_dir)
        
        print("")
        print(chronorage_installer_dir + chronorage_demo_installer_name + " succesfully created.")
    
    
data = PackChronoRageData()
data.packChronoRageData()
data.packChronoRageDict()
data.packChronoRageLevels()
data.packChronoRageShaders()
data.copyBinaries()
data.copyVFS()
data.copyDependencyInstalls()
data.makeInstaller()

demo = PackChronoRageDemoData()
demo.packChronoRageData()
demo.packChronoRageDict()
demo.packChronoRageLevels()
demo.packChronoRageShaders()
demo.copyBinaries()
demo.copyVFS()
demo.copyDependencyInstalls()
demo.makeInstaller()