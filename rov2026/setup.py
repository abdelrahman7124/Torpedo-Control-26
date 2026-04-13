from setuptools import find_packages, setup
import os
from glob import glob

package_name = 'rov2026'

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
            'joy_to_move = rov2026.JoyToMove:main',
            'ros_to_esp = rov2026.RosToEsp:main',
            'esp_to_ros = rov2026.EspToRos:main',
            'rov_controller = rov2026.rov_controller:main',
            'rov_input = rov2026.rov_input:main',
            'thrusters_mixer = rov2026.thrusters_mixer:main',
            'thrusters_factors = rov2026.thrusters_factors:main',
            #'gripper_controller = rov2026.gripper_controller:main',
            'esp_commands_control = rov2026.esp_commands_control:main',
            'factors_gui = rov2026.factors_gui:main',

        ],
    },
)
