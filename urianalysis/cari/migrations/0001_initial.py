# Generated by Django 2.0.2 on 2018-03-27 08:45

from django.db import migrations, models


class Migration(migrations.Migration):

    initial = True

    dependencies = [
    ]

    operations = [
        migrations.CreateModel(
            name='Result',
            fields=[
                ('id', models.AutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('time', models.DateTimeField(auto_now_add=True)),
                ('read', models.CharField(max_length=9999)),
                ('write', models.CharField(max_length=9999)),
                ('dac', models.CharField(max_length=9999)),
            ],
        ),
    ]