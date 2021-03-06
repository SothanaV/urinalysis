# -*- coding: utf-8 -*-

"""
    this code for setup database, Its'll call when setup.sh run
"""
import os
import json
from collections import defaultdict
from django.contrib.auth import get_user_model
from django.db import transaction
from app.models import Device, Task, Team, UserInformation

User = get_user_model()

print()
print('='*40)
_secret_path = os.path.join('secret','setup.json')
print("-- Read secret configuration --")
try:
    with open(_secret_path, 'r') as fp:
        secret_var = json.load(fp)
except FileNotFoundError as err:
    print('********************************************')
    print('!!! Secret File Not Found, use default value')
    print('********************************************')
    secret_var = {
        'admin_email': '',
        'admin_username': 'admin',
        'admin_password': 'qwer1234'
    }
    for k, v in secret_var.items():
        print('\t{}: {}'.format(k, v))
secret_var = defaultdict(str, secret_var)
print()

print("- Create Admin user")
admin_user = User.objects.create_superuser(
    username=secret_var['admin_username'],
    password=secret_var['admin_password'],
    email=secret_var['admin_email'],
)

print()
print('team')
tu_team = Team.objects.create(title='Thammasart')
print('update userinfo')
admin_userinfo = UserInformation.objects.create(user=admin_user, team=tu_team)
print('device')
device = Device.objects.create(team=tu_team)
print('task')
task = Task.objects.create(device=device)
