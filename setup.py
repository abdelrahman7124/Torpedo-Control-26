from setuptools import find_packages, setup
import os
from glob import glob

package_name = 'joystick_control_pkg'

setup(
    name=package_name,
    version='0.0.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
        (os.path.join('share', package_name, 'launch'), glob('launch/*.py')),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='ayman',
    maintainer_email='aymanr2710@gmail.com',
    description='TODO: Package description',
    license='TODO: License declaration',
    extras_require={
        'test': [
            'pytest',
        ],
    },
    entry_points={
        'console_scripts': [
            'joy_to_move = joystick_control_pkg.JoyToMove:main',
            'ros_to_esp = joystick_control_pkg.RosToEsp:main',
            'esp_to_ros = joystick_control_pkg.EspToRos:main',
            'rov_controller = joystick_control_pkg.rov_controller:main',
            'rov_input = joystick_control_pkg.rov_input:main',
            'thrusters_mixer = joystick_control_pkg.thrusters_mixer:main',
        ],
    },
)
